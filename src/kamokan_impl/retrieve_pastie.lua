local full_token = KEYS[1]
local requested_lang = ARGV[1]
local pastie_info = redis.call("HMGET", full_token, "lang", "selfdes")
local pastie_lang = pastie_info[1]
local pastie_selfdes = pastie_info[2]
local ret_pastie = false
local ret_comment = ""
local is_highlighted = 0
if pastie_lang == requested_lang then
	local result = redis.call("HMGET", full_token, "hl_pastie", "hl_comment")
	ret_pastie = result[1]
	ret_comment = result[2]
	is_highlighted = 1
end

if false == ret_pastie then
	local result = redis.call("HMGET", full_token, "pastie")
	ret_pastie = result[1]
	is_highlighted = 0
end

if false == ret_pastie then
	return redis.error_reply("PastieNotFound")
end

local selfdes = 0
local deleted = 0
if pastie_selfdes == "1" then
	deleted = redis.call("DEL", full_token)
	selfdes = 1
end

return {ret_pastie, selfdes, deleted, is_highlighted, pastie_lang, ret_comment}
