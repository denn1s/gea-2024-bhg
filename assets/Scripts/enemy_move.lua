
function update(params)
    local dx = params.playerX - params.enemyX
    local dy = params.playerY - params.enemyY
    local distance = math.sqrt(dx*dx + dy*dy)
    
    -- Define the radius within which enemies stop getting closer
    local stopRadius = 200
    
    -- Move towards the player with a sine wave motion, but only if outside the stop radius
    local baseSpeed = 50
    local speedMultiplier = (distance > stopRadius) and (1 + (distance / 100)) or 0
    local speed = baseSpeed * speedMultiplier * params.deltaTime
    
    -- Calculate the angle to the player
    local angle = math.atan2(dy, dx)
    
    -- Add a sine wave to the movement
    local waveAmplitude = 20  -- Adjust this to change the wave size
    local waveFrequency = 2   -- Adjust this to change how fast the wave oscillates
    local sineOffset = math.sin(params.time * waveFrequency) * waveAmplitude
    
    -- Calculate new position with sine wave
    local moveX = math.cos(angle) * speed
    local moveY = math.sin(angle) * speed
    local perpX = -math.sin(angle) * sineOffset
    local perpY = math.cos(angle) * sineOffset
    
    -- Round the new positions to integers
    local newX = math.floor(params.enemyX + moveX + perpX + 0.5)
    local newY = math.floor(params.enemyY + moveY + perpY + 0.5)
    
    -- Return integer values
    return {
        x = newX,
        y = newY
    }
end
