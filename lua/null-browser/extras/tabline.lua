local html = require 'null-browser.extras.html'
local tabline = {}

-- TODO: vertical tabs
function tabline.init(opts)
  local decoration = (opts or {}).decoration or web.decorations.top

  web.event.add_listener('WinCreated', {
    callback = function(event)
      decoration.enable({ win = event.win_id })
      tabline.show_tabs_in_window(event.win_id, decoration);
    end,
  })

  -- Toggle tabline in current window
  web.keymap.set('n', '<c-b>', function()
    if decoration.is_enabled({ win = 0 }) then
      decoration.disable({ win = 0 })
    else
      decoration.enable({ win = 0 })
    end
  end)
end

function tabline.show_tabs_in_window(win_id, decoration)
  web.event.add_listener({ 'UrlChanged', 'ViewSelected', 'ViewCreated', 'ViewClosed' }, {
    callback = function()
      if not decoration.is_enabled({ win = win_id }) then return end

      local view = decoration.view({ win = win_id })

      web.view.expose('tab_select', function(args)
        print(web.inspect(args))
        local view_id = args and args.view and tonumber(args.view)
        if view_id then
          web.view.select(view_id)
        end
      end, { view = view })

      web.view.set_html(tabline.tabs_html(), { view = view })
    end,
  })
end

function tabline.tabs_html()
  local tab_list = {}
  for index, view in ipairs(web.view.list()) do
    local tab = html.button({
      class = 'tab' .. (web.view.current() == view.id and ' current' or ''),
      onclick = '_nullbrowser.rpc.tab_select({ view: ' .. view.id .. ' })',
    }, {
      index .. ': ' .. view.title .. ' (' .. view.url .. ')',
    })
    table.insert(tab_list, tab)
  end

  local tabs_html = html.div({}, {
    html.style({}, { html.raw(tabline.css()) }),
    html.div({ class = 'tabs' }, tab_list)
  })

  return html.to_string(tabs_html)
end

function tabline.css()
  return [[
    html, body {
      background: #000;
    }
    .tabs {
      display: flex;
      align-items: stretch;
      height: 100vh;
    }
    .tab {
      all: unset;
      flex: 1;
      width: 100%;
      overflow: hidden;
      text-overflow: ellipsis;
      text-wrap: nowrap;
      text-align: left;
      min-width: 0;
      vertical-align: middle;
      padding: 0 8px;
      border-left: 1px solid #555;
    }
    .tab:first-child {
      border: none;
    }
    .tab.current {
      background: #007070;
    }
  ]]
end

return tabline
