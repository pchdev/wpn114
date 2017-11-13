#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include <CoreFoundation/CFBundle.h>
#include <wpn114/audio/plugins/vst/vst.hpp>
#include <wpn114/audio/plugins/vst/vstwindow.h>
#include <iostream>

    // code from Teragon::MrsWatson
    aeffect* wpn114::audio::plugins::vst_hdl::_load_vst_2x_plugin(const char* path)
    {

        std::cout << path << std::endl;
        CFStringRef plugin_path_string_ref = CFStringCreateWithCString(NULL, path, kCFStringEncodingASCII);
        CFURLRef bundle_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, plugin_path_string_ref, kCFURLPOSIXPathStyle, true);

        if(bundle_url == NULL)
        {
            std::cerr << "Couldn't make URL reference for plugin" << std::endl;
            return NULL;
        }

        CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, bundle_url);

        if (bundle == NULL)
        {
            std::cerr << "Couldn't create bundle reference" << std::endl;
            CFRelease(plugin_path_string_ref);
            CFRelease(bundle_url);
            return NULL;
        }

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

    extern "C"
    {

    void wpn114::audio::plugins::vst_hdl::_show_vst_2x_editor
    (aeffect* effect, const char* plugin_name, uint16_t width, uint16_t height)
    {
        NSRect frame;
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        //[NSApplication shared_application];
        //VstWindowAppDelegate *app_delegate = [[[VstWindowAppDelegate alloc] init] autorelease];
        VstWindowAppDelegate *app_delegate = [[NSApplication sharedApplication] delegate];
        [NSApp setDelegate:app_delegate];
        NSApplicationLoad();

        NSRect mainScreenRect = [[NSScreen mainScreen] frame];
        frame.origin.x = (mainScreenRect.size.width - width) / 2;
        frame.origin.y = (mainScreenRect.size.height - height) / 2;
        frame.size.width = width;
        frame.size.height = height;

        NSWindow *window  = [[[NSWindow alloc] initWithContentRect:frame
                                                         styleMask:NSBackingStoreBuffered
                                                           backing:NSBackingStoreBuffered
                                                             defer:NO]
                             autorelease];
        NSRect innerFrame = NSMakeRect(0, 0, width, height);
        NSView *view = [[[NSView alloc] initWithFrame:innerFrame] autorelease];
        [window setContentView:view];
        NSString *windowTitle = [[[NSString alloc] initWithBytes:plugin_name
                                                          length:strlen(plugin_name)
                                                        encoding:NSASCIIStringEncoding]
                                autorelease];
        [window setTitle:windowTitle];
        [window makeKeyAndOrderFront:NSApp];

        std::cerr << "Opening plugin editor window" << std::endl;

        effect->dispatcher(effect, effEditOpen, 0, 0, (void*)view, 0);
        [window orderFrontRegardless];

        std::cerr << "Starting app runloop" << std::endl;

        [NSApp run];

        std::cerr << "App runloop stopped" << std::endl;

        [pool release];
    }

    }
