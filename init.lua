print('hello starting up...')

web.set('new_view_url', 'https://lite.duckduckgo.com')
web.set('close_window_when_no_views', true)
web.set('user_agent', 'MacOS | Safari - $500 edition')
web.set('downloads_dir', os.getenv('HOME') .. '/Downloads/firefox')

local dmenu = require 'null-browser.extras.dmenu'
local history = require 'null-browser.extras.history'
local search_engines = require 'null-browser.extras.search-engines'

history.attach_hooks()

search_engines.urls['ld'] = 'https://lite.duckduckgo.com/?q={}'

require 'null-browser.defaults.vi'.setup {
  menu = dmenu,
  history = history,
  preprocess_url = search_engines.preprocess_url,
}

print('ending...')
