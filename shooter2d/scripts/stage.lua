function StartStage()
	for i = 1, 5 do  -- 5体生成する。
		StartCoroutine('smallBlueEnemy', ScreenWidth / 4, 0, 1)
		for j = 1, 15 do  -- 15フレーム待つ。
			coroutine.yield()
		end
	end
	for i = 1, 120 do
		coroutine.yield()
	end
	for i = 1, 5 do
		StartCoroutine('smallBlueEnemy', ScreenWidth * 3 / 4, 0, -1)
		for j = 1, 15 do
			coroutine.yield()
		end
	end
end

function smallBlueEnemy(initPosX, initPosY, dir)
	local speed = 2.0
	local angle = math.pi / 2
	local enemy = GenerateMover('SmallBlueEnemy', initPosX, initPosY, speed, angle)
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
