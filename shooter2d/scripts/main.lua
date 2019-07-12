--[[local function foo(initPosX, speed)
	for i = 1, 5 do
		local enemy = GenerateEnemy(EnemyID.SmallBlue, initPosX, 0, speed, math.pi / 2, 8)
		for i = 1, 15 do
			coroutine.yield()
		end
	end
	for i = 1, 180 do
		coroutine.yield()
	end
end]]

function Main()
	local stage = require('scripts.stage')
	stage.Start()

	--[[Lua���ŃR���[�`�������s����ꍇ�B
	local co = coroutine.create(foo)
	repeat
		coroutine.resume(co, ScreenWidth / 4, 2)
		coroutine.yield()  -- 1�t���[�����ɌĂяo�����ɕԂ��B
	until coroutine.status(co) == 'dead'
	collectgarbage()]]
end