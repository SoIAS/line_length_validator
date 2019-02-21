#include "line_length_validator.h"
#include <filesystem>

#include <string>

namespace llv
{
	line_length_validator::line_length_validator(const llv::validator_settings& settings, const std::string& root_directory) :
		validator_settings_{ settings },
		root_directory_{ root_directory }
	{

	}


	void line_length_validator::validate(const bool update_directory_files)
	{
		// todo to some check for dirty cache and update only when needed without the variable
		// same thing for overview update
		if (update_directory_files)
		{
			update_files_in_directory();
		}

		for (auto& file_validator : file_validators_)
		{
			file_validator.validate(validator_settings());
		}
	}

	void line_length_validator::update_files_in_directory()
	{
		// todo, or just simply work on the original one?
		auto files_in_directory = files_to_validate();

		std::vector<file_validator> new_file_validators;
		new_file_validators.reserve(files_in_directory.size());

		for(auto& file_path : files_in_directory)
		{
			new_file_validators.emplace_back(std::move(file_path));
		}

		file_validators_ = std::move(new_file_validators);
	}

	void line_length_validator::update_files_overview()
	{
		for(auto& file_validator : file_validators_)
		{
			file_validator.update_overview(validator_settings());
		}
	}

	std::vector<std::string> line_length_validator::files_to_validate() const
	{
		const auto has_extension = [this](const std::string& str) -> bool
		{
			for (const auto& extension : validator_settings().file_extensions_to_validate)
			{
				if (str.length() < extension.length())
				{
					continue;
				}

				if (str.find(extension, str.size() - extension.size()) != std::string::npos)
				{
					return true;
				}
			}

			return false;
		};

		// todo, maybe use directory_entry instead of to get files/store them?
		// refactor later in case we need to check if our data is dirty instead of revalidating everything
		std::vector<std::string> found_files_paths;
		for (auto& directory_entry : std::filesystem::recursive_directory_iterator(root_directory()))
		{
			if (directory_entry.is_regular_file() && has_extension(directory_entry.path().string()))
			{
				found_files_paths.push_back(directory_entry.path().string());
			}
		}

		return found_files_paths;
	}
}
