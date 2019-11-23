local parameters = {
	InvincibleFrames = 360,
	InputDelayFrames = 90,
	ID = PlayerID.Reimu,
	HighSpeed = 4.5,
	LowSpeed = 2.0,
	NormalShot = ShotID.ReimuNormal,
	ShotDelayFrames = 6,
	BulletSpeed = 30.0
}

local player = nil

-- 初期化。
local function Initialize()
	player = GeneratePlayer(parameters.ID, ScreenWidth * 0.5, ScreenHeight + PlayerHeight - parameters.InputDelayFrames)
	player:TurnInvincible(parameters.InvincibleFrames / 2)
	coroutine.yield()
end

-- 自機の復帰処理。
local function Rebirth()
	if (not player:IsEnabled()) and (player.Life > 0) then
		player.PosX = ScreenWidth * 0.5
		player.PosY = ScreenHeight + PlayerHeight
		player:Spawned()
		player:TurnInvincible(parameters.InvincibleFrames);
		coroutine.yield()
		for i = 1, parameters.InputDelayFrames do
			-- ここでSetVelocityを使うと、移動制限処理のところで不具合が生じる。
			GetPlayer().PosY = GetPlayer().PosY - 1.0
			coroutine.yield()
		end
	end
end

-- 自機の移動。復帰との兼ね合いから、Playerクラス内で処理できない。
local function Move()
	while true do
		-- TODO: Playerクラス内のLowSpeed/HighSpeedを使うように修正。SetVelocityの引数を (direction: Vector2, slowMode: bool) に変更。
		local speed = GetKey(CommandID.Slow) and parameters.LowSpeed or parameters.HighSpeed
		local velX, velY = 0.0, 0.0
		if GetKey(CommandID.Leftward) then
			velX = -speed
		end
		if GetKey(CommandID.Rightward) then
			velX = speed
		end
		if GetKey(CommandID.Forward) then
			velY = -speed
		end
		if GetKey(CommandID.Backward) then
			velY = speed
		end
		-- TODO: Vector2クラスで書けるようにする。
		if velX ~= 0.0 or velY ~= 0.0 then
			velX = velX * speed / math.sqrt(velX ^ 2 + velY ^ 2)
			velY = velY * speed / math.sqrt(velX ^ 2 + velY ^ 2)
		end
		player:SetVelocity(velX, velY)
		coroutine.yield()
	end
end

-- 自機のショット。
local function Shoot()
	while true do
		if GetKey(CommandID.Shot) then
			GenerateShot(parameters.NormalShot, player.PosX - 12, player.PosY, parameters.BulletSpeed, -math.pi / 2)
			GenerateShot(parameters.NormalShot, player.PosX + 12, player.PosY, parameters.BulletSpeed, -math.pi / 2)
			for i = 1, parameters.ShotDelayFrames do
				coroutine.yield()
			end
		else	
			coroutine.yield()
		end
	end
end

function PlayerScript()
	Initialize()
	local co = { coroutine.create(Shoot), coroutine.create(Move) }
	repeat
		Rebirth()
		for i = 1, #co do
			coroutine.resume(co[i])
		end
		coroutine.yield()
	until false
end