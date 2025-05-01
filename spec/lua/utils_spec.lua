describe('web.utils.string_trim', function()
  it('trims whitespace from the start and end of string', function()
    expect(web.utils.string_trim('  hello')).to_equal 'hello'
    expect(web.utils.string_trim('hello  ')).to_equal 'hello'
    expect(web.utils.string_trim('  hello  ')).to_equal 'hello'
  end)
end)

describe('web.utils.table_merge', function()
  context('when both tables have integer keys', function()
    it('returns merged table', function()
      expect(web.utils.table_merge({ a = 0 }, { b = 1 }, { c = 2 }))
          .to_equal { a = 0, b = 1, c = 2 }
    end)
  end)

  context('when both tables unique keys', function()
    it('returns merged table', function()
      expect(web.utils.table_merge({ a = 0 }, { b = 1 }, { c = 2 }))
          .to_equal { a = 0, b = 1, c = 2 }
    end)
  end)

  context('when both tables have a key in common', function()
    it('returns merged table', function()
      expect(web.utils.table_merge({ foo = 1, a = 0 }, { foo = 2 }, { foo = 3 }))
          .to_equal { foo = 3, a = 0 }
    end)

    it('mutates the first table', function()
      local tbl = { foo = 1, a = 0 }
      web.utils.table_merge(tbl, { foo = 2 })
      expect(tbl).to_equal { foo = 2, a = 0 }
    end)
  end)
end)

describe('web.utils.equals', function()
  context('when values are primitives', function()
    it('checks for equality', function()
      expect(web.utils.equals(1, 1)).to_be_true()
      expect(web.utils.equals(3, 1)).to_be_false()
      expect(web.utils.equals(nil, 1)).to_be_false()
      expect(web.utils.equals(1, nil)).to_be_false()
      expect(web.utils.equals(nil, nil)).to_be_true()
    end)
  end)

  context('when values are tables', function()
    it('checks for equality', function()
      expect(web.utils.equals({}, 0)).to_be_false()
      expect(web.utils.equals(0, {})).to_be_false()
      expect(web.utils.equals({}, {})).to_be_true()
      expect(web.utils.equals({ a = 1 }, { a = 1 })).to_be_true()
      expect(web.utils.equals({ a = 2 }, { a = 1 })).to_be_false()
      expect(web.utils.equals({}, { a = 1 })).to_be_false()
      expect(web.utils.equals({ a = 1 }, {})).to_be_false()
    end)

    context('when tables are deeply nested', function()
      it('checks for equality', function()
        expect(web.utils.equals({ a = { b = 2 } }, { a = { b = 2 } })).to_be_true()
        expect(web.utils.equals({ a = { b = 3 } }, { a = { b = 2 } })).to_be_false()
      end)
    end)
  end)
end)

describe('web.utils.table_keys', function()
  it('returns keys of given table', function()
    local result = web.utils.table_keys({ a = 1, b = 2 })
    table.sort(result) -- Sort for deterministic ordering
    expect(result).to_equal({ 'a', 'b' })
  end)

  context('with index keys', function()
    it('returns indexes of given table', function()
      expect(web.utils.table_keys({ 'hello', 'world' })).to_equal({ 1, 2 })
      expect(web.utils.table_keys({ 'hello', a = 1, 'world' })).to_equal({ 1, 2, 'a' })
    end)
  end)
end)

describe('web.inspect', function()
  it('returns string representation of value', function()
    expect(web.inspect('hello')).to_equal '"hello"'
    expect(web.inspect(5)).to_equal '5'
    expect(web.inspect({ a = '200', b = 5 })).to_equal '{\n  a = "200",\n  b = 5\n}'
  end)
end)
