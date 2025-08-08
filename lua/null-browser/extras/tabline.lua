local html = require 'null-browser.extras.html'
local tabline = {
  config = { decoration = web.decorations.top }
}

-- TODO: vertical tabs
function tabline.init(opts)
  tabline.config = web.utils.table_merge(tabline.config, opts)
  local decoration = tabline.config.decoration

  web.event.add_listener('WinCreated', {
    callback = function(event)
      decoration.enable({ win = event.win_id })
      tabline.show_tabs_in_window(event.win_id, decoration);

      if tabline.is_vertical() then
        tabline.config.decoration.set_size(200, { win = event.win_id })
      else
        tabline.config.decoration.set_size(20, { win = event.win_id })
      end
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
    html.div({ class = 'tabs' .. (tabline.is_vertical() and ' vertical' or '') }, tab_list)
  })

  return html.to_string(tabs_html)
end

function tabline.is_vertical()
  return web.utils.table_contains({ 'left', 'right' }, tabline.config.decoration.type())
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
    .tabs.vertical {
      flex-direction: column;
      justify-contents: flex-start;
      align-items: flex-start;
    }
    .tabs.vertical .tab {
      border-left: none;
      padding: 6px 3px;
      flex: none;
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
