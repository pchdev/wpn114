extern "C" {

#include "vst.h"
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include <CoreFoundation/CFBundle.h>

    // code from Teragon::MrsWatson
    aeffect* wpn114::audio::vst::plugin_handler::load_vst_2x_plugin(const char* path)
    {
        CFStringRef plugin_path_string_ref = CFStringCreateWithCString(NULL, path, kCFStringEncodingASCII);
        CFURLRef bundle_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, plugin_path_string_ref, kCFURLPOSIXPathStyle, true);

        if(bundle_url == NULL)
        {
            std::cerr << "Couldn't make URL reference for plugin" << std::endl;
            return NULL;
        }

        CFBundleRef bundle_ref = CFBundleCreate(kCFAllocatorDefault, bundle_url);

        if (bundle_ref == NULL)
        {
            std::cerr << "Couldn't create bundle reference" << std::endl;
            CFRelease(plugin_path_string_ref);
            CFRelease(bundle_url);
            return NULL;
        }

        CFRelease(plugin_path_string_ref);
        CFRelease(bundle_url);

        vst_plugin_funcptr main_entry_point = NULL;
        main_entry_point = (vst_plugin_funcptr) CFBundleGetFunctionPointerForName(bundle, CFSTR("VSTPluginMain"));

        if (main_entry_point == NULL)
        {
            std::cerr << "Couldn't get a pointer to the plugin's main" << std::endl;
            CFRelease(plugin_path_string_ref);
            CFRelease(bundle_url);
            return NULL;
        }

        aeffect* plugin = main_entry_point(host_callback);
        if(plugin == NULL)
        {
            std::cerr << "Plugin's main returns null" << std::endl;
            CFRelease(plugin_path_string_ref);
            CFRelease(bundle_url);
            return NULL;
        }

        CFRelease(plugin_path_string_ref);
        CFRelease(bundle_url);
        return plugin;
    }

    void wpn114::audio::vst::plugin_handler::show_vst_2x_editor(aeffect* effect)
    {
        NSRect frame;
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        [NSApplication shared_application];
        VstWindowAppDelegate *app_delegate = [[[VstWindowAppDelegate alloc] init] autorelease];
        [NSApp setDelegate:app_delegate];
        NSApplicationLoad();

        NSRect mainScreenRect = [[NSScreen mainScreen] frame];
        frame.origin.x = (mainScreenRect.size.width - rect->width) / 2;
        frame.origin.y = (mainScreenRect.size.height - rect->height) / 2;
        frame.size.width = rect->width;
        frame.size.height = rect->height;

        NSWindow *window  = [[[NSWindow alloc] initWithContentRect:frame
                                                         styleMask:NSBackingStoreBuffered
                                                           backing:NSBackingStoreBuffered
                                                             defer:NO]
                             autorelease];
        NSRect innerFrame = NSMakeRect(0, 0, rect->width, rect->height);
        NSView *view = [[[NSView alloc] initWithFrame:innerFrame] autorelease];
        [window setContentView:view];
        NSString *windowTitle = [[[NSString alloc] initWithBytes:pluginName->data
                                                          length:strlen(pluginName->data)
                                                        encoding:NSASCIIStringEncoding]
                                autorelease];
        [window setTitle:windowTitle];
        [window makeKeyAndOrderFront:NSApp];
        logDebug("Opening plugin editor window");
        effect->dispatcher(effect, effEditOpen, 0, 0, (void*)view, 0);
        [window orderFrontRegardless];
        logDebug("Starting app runloop");
        [NSApp run];
        logDebug("App runloop stopped");
        [pool release];

    }
}
