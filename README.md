![null-browser banner](./media/banner.png)

# null-browser [WIP]
A simple, fast web browser built using [qtwebengine (chromium)](https://wiki.qt.io/QtWebEngine), that is configured and extended using lua.

## What does it do?
This web browser is just a stack of web views that can be controlled using lua. Don't expect a full featured web browser, expect the **opposite**.

## What does it not do? How can I do it?
- **No url bar**
  - use dmenu, rofi, fzf in a terminal, etc. for url input
- **No tab ui**
  - use dmenu, rofi, etc. to show a list of tabs on key
  - use tabbed (x11) with multiple windows 
  - use window manager with multiple windows
- **No persistant history**
  - save it in a file on UrlChanged event and show as completion for your url input
  - same as above but in a sqlite db instead
- **No splits**
  - use a window manager
- **No tab ui**
  - use a window manager
  - use [tabbed (X11)](https://tools.suckless.org/tabbed/)
- **No buttons**
  - define key bindings
- **No built-in ad blocker**
  - filter host-names
- **No settings page**
  - write some lua
- **No chromium/web extensions**
  - inject javascript into pages from your lua config
- **No built-in cryptocurrency**
  - add it if you want it
- **No built-in AI**
  - add it if you want it

Any feature you want, you implement yourself.

## Who is it for?
I built this for myself and people like me who enjoy tinkering with their tools.
Out of the box, it is almost unusable... by design.

## Install
### Nix flake
- Add this repo to nixos flake config: `inputs.null-browser.url = "github:phenax/null-browser";`
- Or build it in a directory: `nix build github:phenax/null-browser`
- Or run it without installing: `nix run github:phenax/null-browser`
- Or add to profile: `nix profile install github:phenax/null-browser`

### Others
- Clone
- Run `make build` to build it
- `sudo make install PREFIX=/usr/local` to install it

## Configuring
WIP

