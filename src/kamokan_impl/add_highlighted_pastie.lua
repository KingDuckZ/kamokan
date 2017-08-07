local full_token = KEYS[1]
local highlighted_text = ARGV[1]
local comment = ARGV[2]

local saved = redis.call("HMSET", full_token,
	"hl_pastie", highlighted_text,
	"hl_comment", comment
)

if not saved then
	return redis.error_reply("PastieNotSaved")
end
return "1"
