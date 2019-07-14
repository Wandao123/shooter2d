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
	math.randomseed(os.time())
	local stage1 = require('scripts.stage1')
	stage1.Start()
	ChangeScene(SceneID.StageClear)
	local stage2 = require('scripts.stage2')
	stage2.Start()
	ChangeScene(SceneID.AllClear)

	--[[Lua側でコルーチンを実行する場合。
	local co = coroutine.create(foo)
	repeat
		coroutine.resume(co, ScreenWidth / 4, 2)
		coroutine.yield()  -- 1フレーム毎に呼び出し元に返す。
	until coroutine.status(co) == 'dead'
	collectgarbage()]]
end