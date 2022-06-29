#import <Foundation/Foundation.h>
int main(void) {
	NSString *appSupportDir = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES) lastObject];
	//If there isn't an App Support Directory yet ...
	if (![[NSFileManager defaultManager] fileExistsAtPath:appSupportDir isDirectory:NULL]) {
		NSError *error = nil;
		//Create one
		if (![[NSFileManager defaultManager] createDirectoryAtPath:appSupportDir withIntermediateDirectories:YES attributes:nil error:&error]) {
			NSLog(@"%@", error.localizedDescription);
		}
		else {
			// *** OPTIONAL *** Mark the directory as excluded from iCloud backups
			NSURL *url = [NSURL fileURLWithPath:appSupportDir];
			if (![url setResourceValue:@YES
								forKey:NSURLIsExcludedFromBackupKey
								 error:&error])
			{
				NSLog(@"Error excluding %@ from backup %@", url.lastPathComponent, error.localizedDescription);
			}
			else {
				NSLog(@"Yay");
			}
		}
	}
	printf("%s\n", [appSupportDir UTF8String]);
}
