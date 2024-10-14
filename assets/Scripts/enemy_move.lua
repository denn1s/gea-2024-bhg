
function update(params)
    local dx = params.playerX - params.enemyX
    local dy = params.playerY - params.enemyY
    local distance = math.sqrt(dx*dx + dy*dy)
    
    -- Move towards the player, but not too fast
    local speed = 50 * params.deltaTime  -- adjust this value to change enemy speed
    local moveX = dx / distance * speed
    local moveY = dy / distance * speed
    
    -- Round the new positions to integers
    local newX = math.floor(params.enemyX + moveX + 0.5)
    local newY = math.floor(params.enemyY + moveY + 0.5)
    
    -- Return integer values
    return {
        x = newX,
        y = newY
    }
end
