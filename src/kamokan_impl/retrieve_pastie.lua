local full_token = KEYS[1]
local result = redis.call("HMGET", full_token, "pastie", "selfdes", "lang")
if false == result[1] then
	return redis.error_reply("PastieNotFound")
end

local selfdes = 0
local deleted = 0
if result[2] == "1" then
	deleted = redis.call("DEL", full_token)
	selfdes = 1
end

return {result[1], selfdes, deleted, result[3]}
