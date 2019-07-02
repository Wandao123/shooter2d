-- 『東方紅魔郷』1面中ボスまでの再現。ただし、4番目の編隊はランダムなので、原作とは異なる。
function StartStage()
	Wait(120)
	for i = 1, 5 do  -- 5体生成する。
		StartCoroutine(Curve, ScreenWidth / 4, 2, 30, 1, 30)
		Wait(15)
	end
	Wait(120)
	for i = 1, 5 do
		StartCoroutine(Curve, ScreenWidth * 3 / 4, 2, 30, -1, 30)
		Wait(15)
	end
	Wait(120)
	for i = 1, 9 do
		StartCoroutine(Curve, ScreenWidth / 2 - 20 * (10 - i) , 2, 30, -1, 120)
		StartCoroutine(Curve, ScreenWidth / 2 + 20 * (10 - i) , 2, 30, 1, 120)
		Wait(15)
	end
	Wait(120)
	math.randomseed(os.time())
	for i = 1, 15 do
		StartCoroutine(AllDirection, math.random(ScreenWidth / 5, ScreenWidth * 4 / 5), (i % 2 == 0) and (20) or (19))
		Wait(90)
	end
	Wait(10)
	for i = 1, 5 do
		StartCoroutine(Curve, ScreenWidth * 3 / 4 - 15, 2, 30, -1, 60)
		StartCoroutine(Curve, ScreenWidth * 3 / 4 + 15, 2, 60, -1, 60)
		Wait(15)
	end
	Wait(120)
	for i = 1, 5 do
		StartCoroutine(Curve, ScreenWidth / 4 - 15, 2, 60, 1, 60)
		StartCoroutine(Curve, ScreenWidth / 4 + 15, 2, 30, 1, 60)
		Wait(15)
	end
	Wait(300)

	--[[Lua側でコルーチンを実行する場合。
	local co = coroutine.create(smallBlueEnemy)
	repeat
		coroutine.resume(co, ScreenWidth / 4, 0, 1)
		coroutine.yield()
	until coroutine.status(co) == 'dead'
	collectgarbage()]]
end

-- framesに指定されたフレーム数だけ待つ。
function Wait(frames)
	for i = 1, frames do
		coroutine.yield()
	end
end

-- フレーム数frames毎にenemyオブジェクトから自機狙い弾を生成する。
function ShootPer(frames, enemy)
	local MaxFrames = 120
	for i = 0, MaxFrames do
		if i % frames == 0 then
			GenerateBullet(BulletID['Small'], enemy, 4,
				math.pi / 2 - math.atan(GetPlayer():GetPosX() - enemy:GetPosX(), GetPlayer():GetPosY() - enemy:GetPosY()))
		end
		coroutine.yield()
	end
end

-- 始めは真っ直ぐに進み、途中で横に曲がる敵。途中で自機狙い弾を発射。
-- initPosX: 初期位置; speed: 速さ; radius: 曲がる際に描く円の半径; dir: 曲がる方向（正負）; delay: ショットの間隔.
function Curve(initPosX, speed, radius, dir, delay)
	dir = dir / math.abs(dir)
	local angle = math.pi / 2
	local enemy = GenerateEnemy(EnemyID['SmallBlue'], initPosX, 0, speed, angle, 8)
	coroutine.yield()
	StartCoroutine(ShootPer, delay, enemy)
	Wait(120)
	-- 1フレームで進む長さ＝speed。これと円の半径を二辺とする二等辺三角形に対して、余弦定理を適用する。
	local diffAngle = math.acos(1.0 - 0.5 * speed ^ 2 / radius ^ 2)
	angle = angle - dir * diffAngle / 2  -- 曲がり始めはキャラの初期角度も考慮する。
	repeat
		enemy:SetAngle(angle)
		angle = angle - dir * diffAngle  -- 曲がっている途中は正確にこの値だけずれてゆく。
		coroutine.yield()
	until (dir >= 0) and (angle < 0.0) or (angle > math.pi)  -- 三項演算子
end

-- 真っ直ぐ降りてきて、全方位弾を発射してから引き返す敵。
-- initPosX: 初期位置; ways: 弾数.
function AllDirection(initPosX, ways)
	local enemy = GenerateEnemy(EnemyID['SmallBlue'], initPosX, 0, 1.5, math.pi / 2, 8)
	Wait(100)
	enemy:SetSpeed(0)
	Wait(5)
	local diffAngle = 2 * math.pi / ways
	local startingAngle = 0
	if ways % 2 == 0 then
		-- 自機の方向から diffAngle / 2 だけずれた方向。
		startingAngle = math.pi / 2 - math.atan(GetPlayer():GetPosX() - enemy:GetPosX(), GetPlayer():GetPosY() - enemy:GetPosY()) - diffAngle / 2
	else
		-- 自機の方向
		startingAngle = math.pi / 2 - math.atan(GetPlayer():GetPosX() - enemy:GetPosX(), GetPlayer():GetPosY() - enemy:GetPosY())
	end
	for i = 0, ways - 1 do
		GenerateBullet(BulletID['Small'], enemy, 1, startingAngle + i * diffAngle)
	end
	Wait(15)
	local dir = (initPosX < ScreenWidth / 2) and (1) or (-1)  -- 初期位置が左寄りなら右向きに、右寄りなら左向きに進む。
	enemy:SetSpeed(3)
	for i = 1, 4 do
		enemy:SetAngle(math.pi / 2 - dir * i * math.pi / 4)
		Wait(6)
	end
end
