---@diagnostic disable: doc-field-no-class, undefined-doc-param, undefined-doc-name
--- Lua api for configuring null browser.
--- @module web

-- TODO: Make all opts naming consistent

--- Events that can be listened to with web.events.add_listener
---
--- @table WebEvents
--- @see KeyPressedEventOpts
--- @see ModeChangedEventOpts
--- @see NotificationReceivedEventOpts
--- @see PermissionRequestedEventOpts
--- @see UrlChangedEventOpts
--- @see ViewClosedEventOpts
--- @see ViewCreatedEventOpts
--- @see ViewSelectedEventOpts
--- @see WinCreatedEventOpts
--- @see web.event.add_listener
---
--- @usage
--- web.events.add_listener('UrlChanged', {
---   callback = function(opts)
---     print(opts.url)
---   },
--- end)

--- @table WinCreatedEventOpts
--- @field type 'WinCreated'
--- @field view integer
--- @field win integer

--- @table ViewCreatedEventOpts
--- @field type 'ViewCreated'
--- @field view integer
--- @field win integer

--- @table ViewSelectedEventOpts
--- @field type 'ViewSelected'
--- @field view integer
--- @field win integer

--- @table ViewClosedEventOpts
--- @field type 'ViewClosed'
--- @field view integer
--- @field win integer

--- @table UrlChangedEventOpts
--- @field type 'UrlChanged'
--- @field view_id integer
--- @field win_id integer
--- @field url string

--- @table PermissionRequestedEventOpts
--- @field type 'PermissionRequested'
--- @field premission_type 'MediaAudioCapture' | 'MediaVideoCapture' | 'MediaAudioVideoCapture' | 'DesktopVideoCapture' | 'DesktopAudioVideoCapture' | 'MouseLock' | 'Notifications' | 'Geolocation' | 'ClipboardReadWrite' | 'LocalFontsAccess' | 'Unsupported'
--- @field webview_id integer
--- @field win_id integer
--- @field accept fun():nil
--- @field reject fun():nil

--- @table ModeChangedEventOpts
--- @field type 'ModeChanged'
--- @field mode string

--- @table KeyPressedEventOpts
--- @field type 'KeyPressed'
--- @field key string

--- @table NotificationReceivedEventOpts
--- @field type 'NotificationReceived'
--- @field title string
--- @field message string
--- @field tag string
--- @field origin string
--- @field click fun():nil
--- @field show fun():nil
--- @field close fun():nil

