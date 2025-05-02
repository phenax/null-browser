print('hello starting up...')

web.set('new_view_url', 'https://lite.duckduckgo.com')
web.set('close_window_when_no_views', true)
web.set('user_agent',
  'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36')
web.set('downloads_dir', os.getenv('HOME') .. '/Downloads/firefox')

local history = require 'null-browser.extras.history'
history.attach_hooks()

local search_engines = require 'null-browser.extras.search-engines'
search_engines.urls['ld'] = 'https://lite.duckduckgo.com/?q={}'

local Dmenu = require 'null-browser.extras.dmenu'
local menu = Dmenu:new {
  command = os.getenv('HOME') .. '/scripts/fzfmenu.sh',
  prompt_arg = '--prompt',
  query_arg = '-q',
  args = { 'input', '--layout=default' },
}

require 'null-browser.defaults.vi'.setup {
  menu = menu,
  history = history,
  transform_url_input = search_engines.transform_url_input,
}

web.set('permissions_persistance', 'never')

web.event.add_listener('NotificationReceived', {
  callback = function(event)
    -- TODO: Add action/click (-A)
    local args = { '-a', 'null-browser', '-r', event.tag, event.title, event.message }
    web.uv.spawn('notify-send', { args = args }, function(code)
      if code ~= 0 then
        print('[notify-send] Exit with status code: ' .. code)
      end
    end)
  end,
})

print('ending...')
