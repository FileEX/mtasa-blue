require 'utils'

premake.modules.install_unifont = {}

-- Config variables
local UNIFONT_BASEURL = "https://github.com/multitheftauto/unifont/releases/download/"
local UNIFONT_DOWNLOAD_FILENAME = "unifont-16.0.04.ttf"
local UNIFONT_FILENAME = "unifont.ttf"
local UNIFONT_PATH = "Shared/data/MTA San Andreas/MTA/cgui"

-- Change these to update the version
local UNIFONT_TAG = "v16.0.04"
local UNIFONT_HASH = "6b18fdcf0f61f3df2e61db9737af6de9f37048ec93083621d41504ebf8cb18f4"

newaction {
	trigger = "install_unifont",
	description = "Downloads and installs Unifont",

	execute = function()
		-- Check file hash
		local archive_path = UNIFONT_PATH.."/"..UNIFONT_FILENAME
		if os.isfile(archive_path) and os.sha256_file(archive_path) == UNIFONT_HASH then
			print("Unifont is up to date.")
			return
		end

		-- Download Unifont
		print("Downloading Unifont...")
		if not http.download_print_errors(UNIFONT_BASEURL..UNIFONT_TAG.."/"..UNIFONT_DOWNLOAD_FILENAME, archive_path) then
			os.exit(1)
			return
		end

		-- Check downloaded file hash
		local download_hash = os.sha256_file(archive_path)
		if download_hash ~= UNIFONT_HASH then
			errormsg("ERROR: Unifont hash mismatch!", ("\nExpected %s, got %s"):format(UNIFONT_HASH, download_hash))
			-- Delete bad file
			os.remove(archive_path)
			os.exit(1)
			return
		end

		print("Unifont updated.")
	end
}

return premake.modules.install_unifont
