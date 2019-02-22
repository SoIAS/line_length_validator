#pragma once

#include <array>
#include <string_view>
#include <charconv>
#include <optional>

#include "validator_settings.h"
#include <iostream>

enum e_argument
{
	argument_help,
	argument_path,
	argument_warning_line_length,
	argument_error_line_length,
	argument_tab_to_spaces,
	argument_print_absolute_file_location,
	argument_file_extensions,
	argument_max
};

inline const std::array<std::string_view, argument_max> possible_arguments{
	"--help",
	"-path",
	"-wlen",
	"-elen",
	"-tsize",
	"-absolute",
	"-extensions",
};

struct parsed_arguments
{
	std::string file_path;
	llv::validator_settings settings;
};


e_argument is_an_argument(const char* arg);
std::optional<int> parse_to_int(const char* arg);

void display_help();

bool has_help_argument(int argc, const char *const *argv);

parsed_arguments parse_arguments(int argc, const char *const *argv);