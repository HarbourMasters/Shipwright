//
//  OSXFolderManager.h
//  libultraship
//
//  Created by David Chavez on 28.06.22.
//

#ifndef OSXFolderManager_h
#define OSXFolderManager_h

#include <stdio.h>
namespace Ship {
    enum {
        NSApplicationDirectory = 1,
        NSDemoApplicationDirectory,
        NSDeveloperApplicationDirectory,
        NSAdminApplicationDirectory,
        NSLibraryDirectory,
        NSDeveloperDirectory,
        NSUserDirectory,
        NSDocumentationDirectory,
        NSDocumentDirectory,
        NSCoreServiceDirectory,
        NSAutosavedInformationDirectory = 11,
        NSDesktopDirectory = 12,
        NSCachesDirectory = 13,
        NSApplicationSupportDirectory = 14,
        NSDownloadsDirectory = 15,
        NSInputMethodsDirectory = 16,
        NSMoviesDirectory = 17,
        NSMusicDirectory = 18,
        NSPicturesDirectory = 19,
        NSPrinterDescriptionDirectory = 20,
        NSSharedPublicDirectory = 21,
        NSPreferencePanesDirectory = 22,
        NSApplicationScriptsDirectory = 23,
        NSItemReplacementDirectory = 99,
        NSAllApplicationsDirectory = 100,
        NSAllLibrariesDirectory = 101,
        NSTrashDirectory = 102
    };
    typedef unsigned long SearchPathDirectory;

    enum {
        NSUserDomainMask = 1,       // user's home directory --- place to install user's personal items (~)
        NSLocalDomainMask = 2,      // local to the current machine --- place to install items available to everyone on this machine (/Library)
        NSNetworkDomainMask = 4,    // publically available location in the local area network --- place to install items available on the network (/Network)
        NSSystemDomainMask = 8,     // provided by Apple, unmodifiable (/System)
        NSAllDomainsMask = 0x0ffff  // all domains: all of the above and future items
    };
    typedef unsigned long SearchPathDomainMask;

    class FolderManager {
        public:
        const char *pathForDirectory(SearchPathDirectory directory, SearchPathDomainMask domainMask);
        const char *pathForDirectoryAppropriateForItemAtPath(SearchPathDirectory directory, SearchPathDomainMask domainMask, const char *itemPath, bool create = false);
    };
};

#endif /* OSXFolderManager_h */
