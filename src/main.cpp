#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include "base64.h"
#include "window.h"
#include "shellapi.h" // for CommandLineToArgvW
#include "commctrl.h" // for InitCommonControlsEx
#include "Shlwapi.h" // for DLLVERSIONINFO2
#include "str_convert.h"

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	return 0;
}

std::optional<std::array<std::wstring, 2>>
get_filenames_from_commandline()
{
	int argc = 0;
	wchar_t** argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if (argc > 3) {
		LocalFree(argv);
		return std::nullopt;
	}
	
	std::wstring input_filename, output_filename;
	switch (argc) {
	case 1:
		std::wcout << L"No input nor output files specified. Using defaults.\n";
		input_filename = L"input.asav";
		output_filename = L"output.dec";
	break;
	case 2:
		std::wcout << L"No output file specified. Using default.\n";
		input_filename = argv[1];
		output_filename = L"output.dec";
	break;
	case 3:
		input_filename = argv[1];
		output_filename = argv[2];
	break;
	}
	
	LocalFree(argv);
	return std::optional{std::array{input_filename, output_filename}};
}

std::wstring read_input_file(const std::wstring& filename)
{
	std::wifstream file(wstring_to_string(filename), std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("the input file doesn't exists.");

	int length = file.tellg();
	std::wstring buffer(length, L'\0');
	file.seekg(0);

	std::wcout << L"length: " << length << L'\n';
	file.read(buffer.data(), length);

	std::wcout << L"File size: " << length << L'\n';
	std::wcout << file.gcount() << L" characters were read.\n";

	return buffer;
}

void write_output_file(const std::wstring& filename, const std::wstring& data)
{
	if (data.size() == 0)
		throw std::runtime_error("there's no data to write to the output file.");

	std::wofstream file(wstring_to_string(filename), std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("couldn't write to the output file.");

	file.write(data.c_str(), data.size());
}

int wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{/*
	INITCOMMONCONTROLSEX icc{sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES};
	if (!InitCommonControlsEx(&icc)) {
		std::wcout << L"Error when trying to load common controls.\n";
		return 1;
	}*/
	
	std::wstring encoded_contents, decoded_contents;
	
	auto ret = get_filenames_from_commandline();
	if (!ret) {
		std::wcerr << L"Wrong number of arguments. Usage: decode.exe [input file] [output file]\n";
		return 1;
	}
	auto [input_file, output_file] = ret.value();
	
	// Read data in base64 format from input file
	try {
		encoded_contents = read_input_file(input_file);
	}
	catch (const std::exception& e) {
		std::wcerr << L"Error: " << e.what() << '\n';
		return 1;
	}

	WNDCLASSEX class_main_window = {sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WindowProcedure, 0, 0, hInstance, nullptr,
		nullptr, HBRUSH(COLOR_WINDOW + 1), nullptr, L"class_main_window", nullptr};
	Window main_window(class_main_window);
	
	decoded_contents = base64_decode(encoded_contents);
	
	// Write decoded data to output file
	try {
		write_output_file(output_file, decoded_contents);
	}
	catch (const std::exception& e) {
		std::wcerr << L"Error: " << e.what() << "\n";
		return 1;
	}
	
	HMENU edit_identifier = (HMENU)1;
	int edit_styles = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_HSCROLL
		| WS_VSCROLL | ES_MULTILINE;
	HWND edit_handle = CreateWindowEx(0, L"EDIT", L"Type here", edit_styles,
	0, 0, 450, 450, main_window.getHandle(), edit_identifier, hInstance, nullptr);
	if (!edit_handle) {
		std::wcerr << L"Error: couldn't create edit control child.\n";
		return 1;
	}
	
	ShowWindow(main_window.getHandle(), nCmdShow);
	// Message loop
	MSG message;
	int message_error_state = 0;
	while ((message_error_state = GetMessage(&message, nullptr, 0, 0))) {
		if (message_error_state == -1) {
			std::wcerr << L"Error: couldn't get message from queue.\n";
			return 1;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return message.wParam;
}