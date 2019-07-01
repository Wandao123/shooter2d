function StartStage()
	for i = 1, 5 do  -- 5体生成する。
		StartCoroutine(SmallBlueEnemy1, ScreenWidth / 4, 0, 1)
		for j = 1, 15 do  -- 15フレーム待つ。
			coroutine.yield()
		end
	end
	for i = 1, 120 do
		coroutine.yield()
	end
	for i = 1, 5 do
		StartCoroutine(SmallBlueEnemy1, ScreenWidth * 3 / 4, 0, -1)
		for j = 1, 15 do
			coroutine.yield()
		end
	end
	for i = 1, 120 do
		coroutine.yield()
	end
	for i = 1, 9 do
		StartCoroutine(SmallBlueEnemy2, ScreenWidth / 2 - 20 * (10 - i) , 0, 1)
		StartCoroutine(SmallBlueEnemy2, ScreenWidth / 2 + 20 * (10 - i) , 0, -1)
		for j = 1, 15 do
			coroutine.yield()
		end
	end
	for i = 1, 300 do
		coroutine.yield()
	end

	--[[Lua側でコルーチンを実行する場合。
	local co = coroutine.create(smallBlueEnemy)
	repeat
		coroutine.resume(co, ScreenWidth / 4, 0, 1)
		coroutine.yield()
	until coroutine.status(co) == 'dead'
	collectgarbage()]]
end

function ShootPer(frames, enemy)
	local MaxFrames = 120
	for i = 0, MaxFrames do
		if i % frames == 0 then
			GenerateBullet(BulletID['Small'], enemy, 4.0,
				math.pi / 2 - math.atan(GetPlayer():GetPosX() - enemy:GetPosX(), GetPlayer():GetPosY() - enemy:GetPosY()))
		end
		coroutine.yield()
	end
end

function SmallBlueEnemy1(initPosX, initPosY, dir)
	local speed = 2.0
	local angle = math.pi / 2
	local enemy = GenerateEnemy(EnemyID['SmallBlue'], initPosX, initPosY, speed, angle, 10)
	coroutine.yield()
	StartCoroutine(ShootPer, 30, enemy)
	for i = 1, 120 do
		coroutine.yield()
	end
	local radius = 30.0
	-- 1フレームで進む長さ＝speed。これと円の半径を二辺とする二等辺三角形に対して、余弦定理を適用する。
	local diffAngle = math.acos(1.0 - 0.5 * speed ^ 2 / radius ^ 2)
	angle = angle - dir * diffAngle / 2  -- 曲がり始めはキャラの初期角度も考慮する。
	repeat
		enemy:SetAngle(angle)
		angle = angle - dir * diffAngle  -- 曲がっている途中は正確にこの値だけずれてゆく。
		coroutine.yield()
	until (dir >= 0) and (angle < 0.0) or (angle > math.pi)  -- 三項演算子
end

function SmallBlueEnemy2(initPosX, initPosY, dir)
	local speed = 2.0
	local angle = math.pi / 2
	local enemy = GenerateEnemy(EnemyID['SmallBlue'], initPosX, initPosY, speed, angle, 10)
	for i = 1, 120 do
		coroutine.yield()
	end
	local radius = 30.0
	local diffAngle = math.acos(1.0 - 0.5 * speed ^ 2 / radius ^ 2)
	angle = angle + dir * diffAngle / 2
	repeat
		enemy:SetAngle(angle)
		angle = angle + dir * diffAngle
		coroutine.yield()
	until (dir < 0) and (angle < 0.0) or (angle > math.pi)
end