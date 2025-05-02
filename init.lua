print('hello starting up...')

local Dmenu = require 'null-browser.extras.dmenu'
local history = require 'null-browser.extras.history'
local search_engines = require 'null-browser.extras.search-engines'

web.set('new_view_url', 'https://lite.duckduckgo.com')
web.set('close_window_when_no_views', true)
web.set('user_agent',
  'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36')
web.set('downloads_dir', os.getenv('HOME') .. '/Downloads/firefox')

history.attach_hooks()

search_engines.urls['ld'] = 'https://lite.duckduckgo.com/?q={}'

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
web.event.add_listener('PermissionRequested', {
  callback = function(event)
    menu:select({ 'Allow', 'Deny' }, { prompt = 'Requesting permission for ' .. event.permission_type },
      function(err, choice)
        if err then return end
        if web.utils.string_trim(choice) == 'Allow' then
          event.accept()
        else
          event.reject()
        end
      end)
  end,
})

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
