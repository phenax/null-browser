local tabline = {}

-- TODO: click interaction
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
  web.event.add_listener('UrlChanged', {
    callback = function()
      if not decoration.is_enabled({ win = win_id }) then return end

      web.view.set_html(tabline.tabs_html(), {
        view = decoration.view({ win = win_id }),
      })
    end,
  })
end

function tabline.tabs_html()
  local views_html = ''
  for index, view in ipairs(web.view.list()) do
    -- TODO: Sanitize this stuff
    local text = index .. ': ' .. view.title .. ' (' .. view.url .. ')'
    local classes = 'tab'
    if web.view.current() == view.id then classes = classes .. ' current' end
    local html = '<div class="' .. classes .. '">' .. text .. '</div>'
    views_html = views_html .. html
  end

  return '<style>' .. tabline.css() .. '</style>' ..
      '<div class="tabs">' .. views_html .. '</div>'
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
      flex: 1;
      width: 100%;
      overflow: hidden;
      text-overflow: ellipsis;
      text-wrap: nowrap;
      min-width: 0;
      border-left: 1px solid white;
    }
    .tab.current {
      background: #333;
    }
  ]]
end

return tabline
