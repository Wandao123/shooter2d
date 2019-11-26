local parameters = {
	InvincibleFrames = 360,
	InputDelayFrames = 90,
	ID = PlayerID.Reimu,
	NormalShot = BulletID.ReimuNormal,
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

-- 自機の移動。復帰との兼ね合い（復帰中は入力を受け付けない）から、Playerクラス内で処理できない。
local function Move()
	while true do
		local dirX, dirY = 0.0, 0.0
		if GetKey(CommandID.Leftward) then
			dirX = -1
		end
		if GetKey(CommandID.Rightward) then
			dirX = 1
		end
		if GetKey(CommandID.Forward) then
			dirY = -1
		end
		if GetKey(CommandID.Backward) then
			dirY = 1
		end
		player:SetVelocity(dirX, dirY, GetKey(CommandID.Slow))
		coroutine.yield()
	end
end

-- 自機のショット。
local function Shoot()
	while true do
		if GetKey(CommandID.Shot) then
			GeneratePlayerBullet(parameters.NormalShot, player.PosX - 12, player.PosY, parameters.BulletSpeed, -math.pi / 2)
			GeneratePlayerBullet(parameters.NormalShot, player.PosX + 12, player.PosY, parameters.BulletSpeed, -math.pi / 2)
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