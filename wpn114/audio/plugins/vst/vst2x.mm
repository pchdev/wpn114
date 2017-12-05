#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include <CoreFoundation/CFBundle.h>
#include <wpn114/audio/plugins/vst/vst.hpp>
#include <wpn114/audio/plugins/vst/vstwindow.h>
#include <iostream>

using namespace wpn114::audio::plugins;
    // code from Teragon::MrsWatson
    aeffect* vst_hdl::_load_vst_2x_plugin(const char* path)
    {
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

    void vst_hdl::_create_vst_2x_editor(uint16_t width, uint16_t height)
    {
        NSRect frame;

        VstWindowAppDelegate *app_delegate = [[NSApplication sharedApplication] delegate];
        [NSApp setDelegate:app_delegate];
        NSApplicationLoad();

        NSRect mainScreenRect   = [[NSScreen mainScreen] frame];
        frame.origin.x          = (mainScreenRect.size.width - width) / 2;
        frame.origin.y          = (mainScreenRect.size.height - height) / 2;
        frame.size.width        = width;
        frame.size.height       = height;

        NSWindow* window  = [[[NSWindow alloc] initWithContentRect:frame
                                                         styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                                           backing:NSBackingStoreBuffered
                                                             defer:NO] autorelease];

        NSRect innerFrame = NSMakeRect(0, 0, width, height);
        NSView *view = [[[NSView alloc] initWithFrame:innerFrame] autorelease];
        [window setContentView:view];

        m_dispatcher(m_plugin, effEditOpen, 0, 0, (void*)view, 0);
        [window makeKeyAndOrderFront:NSApp];

        m_editwindow    = (void*) window;
        m_editview      = (void*) view;

        [NSApp run];
    }

    void vst_hdl::_show_vst_2x_editor()
    {
        NSWindow* window = (NSWindow*) m_editwindow;
        m_plugin->dispatcher(m_plugin, effEditOpen, 0, 0, m_editview, 0);
        [window orderFrontRegardless];
    }

    void vst_hdl::_close_vst_2x_editor()
    {
        NSWindow* window = (NSWindow*) m_editwindow;
        m_plugin->dispatcher(m_plugin, effEditClose, 0, 0, nullptr, 0);
        [window close];

        if(m_editthread.joinable())
            m_editthread.join();
    }
    }
