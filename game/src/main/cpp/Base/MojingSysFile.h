#ifndef MJ_SysFile_h
#define MJ_SysFile_h

#include "MojingFile.h"

namespace Baofeng
{
	namespace Mojing
	{

		// ***** Declared classes
		class   SysFile;

		//-----------------------------------------------------------------------------------
		// *** File Statistics

		// This class contents are similar to _stat, providing
		// creation, modify and other information about the file.
		struct FileStat
		{
			// No change or create time because they are not available on most systems
			SInt64  ModifyTime;
			SInt64  AccessTime;
			SInt64  FileSize;

			bool operator== (const FileStat& stat) const
			{
				return ((ModifyTime == stat.ModifyTime) &&
					(AccessTime == stat.AccessTime) &&
					(FileSize == stat.FileSize));
			}
		};

		//-----------------------------------------------------------------------------------
		// *** System File

		// System file is created to access objects on file system directly
		// This file can refer directly to path.
		// System file can be open & closed several times; however, such use is not recommended
		// This class is realy a wrapper around an implementation of File interface for a 
		// particular platform.

		class SysFile : public DelegatedFile
		{
		protected:
			SysFile(const SysFile &source) : DelegatedFile() { MJ_UNUSED(source); }
		public:

			// ** Constructor
			SysFile();
			// Opens a file
			SysFile(const String& path, int flags = Open_Read | Open_Buffered, int mode = Mode_ReadWrite);

			// ** Open & management 
			bool  Open(const String& path, int flags = Open_Read | Open_Buffered, int mode = Mode_ReadWrite);

			MJ_FORCE_INLINE bool  Create(const String& path, int mode = Mode_ReadWrite)
			{
				return Open(path, Open_ReadWrite | Open_Create, mode);
			}

			// Helper function: obtain file statistics information. In OVR, this is used to detect file changes.
			// Return 0 if function failed, most likely because the file doesn't exist.
			static bool MJ_CDECL GetFileStat(FileStat* pfileStats, const String& path);

			// ** Overrides
			// Overridden to provide re-open support
			virtual int   GetErrorCode();

			virtual bool  IsValid();

			virtual bool  Close();
		};
	}
} // Namespace OVR

#endif
