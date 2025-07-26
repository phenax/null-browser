local statusline = {}

local mode_labels = { n = "NORMAL", i = "INSERT" }

function statusline.init(opts)
  local decoration = (opts or {}).decoration or web.decorations.bottom

  web.event.add_listener('WinCreated', {
    callback = function(event)
      decoration.enable({ win = event.win_id })
      statusline.show_status_in_window(event.win_id, decoration)
    end,
  })

  web.keymap.set('n', '<space>gg', function()
    if decoration.is_enabled({ win = 0 }) then
      decoration.disable({ win = 0 })
    else
      decoration.enable({ win = 0 })
    end
  end)
end

function statusline.show_status_in_window(win_id, decoration)
  local show_statusline = function()
    local view = decoration.view({ win = win_id })
    if view == nil then return end
    web.view.set_html(statusline.html(), { view = view })
  end

  web.schedule(function() show_statusline() end)
  web.event.add_listener('ModeChanged', {
    callback = function(_) show_statusline() end,
  })
end

local mode_styles = {
  n = "background-color: #007070; color: white;",
  i = "background-color: #e06c75; color: white;",
}

function statusline.html()
  local mode = web.keymap.get_mode()
  local mode_label = mode_labels[mode] or mode
  local styles = '<style>' .. statusline.css() .. '</style>'
  local mode_style = mode_styles[mode] or ""
  local html = '<div class="status">' ..
      '<div class="mode" style="' .. mode_style .. '">' .. mode_label .. '</div>' ..
      '</div>'
  return styles .. html
end

function statusline.css()
  return [[
  .status {
    width: 100%;
    height: 100vh;
    display: flex;
    justify-content: flex-start;
    align-items: stretch;
    background-color: #000;
  }
  .mode {
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 0 8px;
    font-weight: bold;
    background-color: #888;
  }
  ]]
end

return statusline
