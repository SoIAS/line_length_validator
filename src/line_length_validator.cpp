#include "line_length_validator.h"
#include <filesystem>

#include <string>

namespace llv
{
	line_length_validator::line_length_validator(llv::validator_settings settings, std::filesystem::path path) :
		validator_settings_{ std::move(settings) },
		path_{ std::move(path) }
	{

	}

	void line_length_validator::validate(const size_t index)
	{
		if (file_validators().size() >= index)
		{
			return; // todo
		}

		if (!file_validator(index).is_validation_cache_up_to_date())
		{
			file_validator(index).validate(validator_settings());
		}
	}


	void line_length_validator::validate(const bool update_directory_files)
	{
		// todo to some check for dirty cache and update only when needed without the variable (the files in directory, not an actual file)
		// same thing for overview update
		if (update_directory_files)
		{
			update_files_in_directory();
		}

		for (auto& file_validator : file_validators_)
		{
			if (!file_validator.is_validation_cache_up_to_date())
			{
				file_validator.validate(validator_settings());
			}
		}
	}

	void line_length_validator::update_overview(const size_t index)
	{
		if (file_validators().size() >= index)
		{
			return; // todo
		}

		if (!file_validator(index).is_overview_cache_up_to_date())
		{
			file_validator(index).update_overview(validator_settings());
		}
	}

	void line_length_validator::update_overview()
	{
		// todo, update directories?

		for (auto& file_validator : file_validators_)
		{
			if (!file_validator.is_overview_cache_up_to_date())
			{
				file_validator.update_overview(validator_settings());
			}
		}
	}

	void line_length_validator::update_files_in_directory()
	{
		if (!is_path_valid())
		{
			return; // todo, return type error
		}

		// todo, or just simply work on the original one?
		auto files_in_directory = files_to_validate();

		std::vector<llv::file_validator> new_file_validators;
		new_file_validators.reserve(files_in_directory.size());

		for (auto& file_path : files_in_directory)
		{
			new_file_validators.emplace_back(std::move(file_path));
		}

		file_validators_ = std::move(new_file_validators);
	}

	std::vector<std::filesystem::path> line_length_validator::files_to_validate() const
	{
		// Passing a path to a file instead of a directory does not require extensions
		// Nor will abide any provided ones
		if (is_regular_file(path()))
		{
			return { path() };
		}

		// todo, maybe use directory_entry instead of to get files/store them?
		// refactor later in case we need to check if our data is dirty instead of re-validating everything
		std::vector<std::filesystem::path> found_files_paths;
		const auto& extensions = validator_settings().file_extensions_to_validate;
		for (auto& directory_entry : std::filesystem::recursive_directory_iterator(path(), std::filesystem::directory_options::skip_permission_denied))
		{
			if (directory_entry.is_regular_file() &&
				std::find(extensions.begin(), extensions.end(), directory_entry.path().extension()) != extensions.end())
			{
				found_files_paths.push_back(directory_entry.path());
			}
		}

		return found_files_paths;
	}
}