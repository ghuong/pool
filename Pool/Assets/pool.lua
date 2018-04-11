-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

red = gr.material({1.0, 0.0, 0.0}, {0.1, 1.0, 0.1}, 100)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {1.1, 1.1, 0.1}, 1)
chrome = gr.material({0.4, 0.6, 0.7}, {1, 20, 40}, 30)
glossy = gr.material({1.0, 1.0, 1.0}, {0.3, 0.2, 0.4}, 50)
diffuse = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 0)

glossywhite = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.5}, 50)
glossyblue = gr.material({0.0, 0.0, 1.0}, {1.0, 1.0, 1.5}, 50)
glossyred = gr.material({1.0, 0.0, 0.0}, {1.0, 1.0, 1.5}, 50)
glossygreen = gr.material({0.0, 1.0, 0.0}, {1.0, 1.0, 1.5}, 50)
glossypurple = gr.material({0.7, 0.0, 1.0}, {1.0, 1.0, 1.5}, 50)
glossyblack = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.5}, 50)
glossyyellow = gr.material({1.0, 1.0, 0.0}, {1.0, 1.0, 1.5}, 50)

function inverse(arr)
  --[[ Precond: arr has no zero entries ]]
  newarr = {}
  for i, v in ipairs(arr) do
    newarr[i] = 1.0 / arr[i]
  end
  return newarr
end

function multEach(arr, factor)
  newarr = {}
  for i, v in ipairs(arr) do
    newarr[i] = arr[i] * factor;
  end
  return newarr
end

function scale(node, scaleVec3)
  --[[
  Scale the node
  scaleVec3: list of size 3: (x, y ,z) scaling
  ]]
  node:scale(table.unpack(scaleVec3))
end

function unscale(node, scaleVec3)
  --[[
  Unscale the node by a dict
  (useful for removing parent node scaling)
  ]]
  node:scale(table.unpack(inverse(scaleVec3)))
end

function rotate(node, rotVec3)
  if rotVec3 then
    node:rotate('x', rotVec3[1])
    node:rotate('y', rotVec3[2])
    node:rotate('z', rotVec3[3])
  end
end

rootnode = gr.node('root')

-- Pool Table ==============================================================

poolScale = {30.0, 1.0, 60.0} -- scale of poolsurface
poolHeight = 15.0 -- y-coord of poolsurface

-- Edges of Pool table constants
edgeHeight = 1.5 -- height(extent) of edges
edgeMoveUp = poolHeight + (edgeHeight + poolScale[2]) / 2.0

-- Pool Surface
poolsurface = gr.mesh('patterncube', 'poolsurface')
rootnode:add_child(poolsurface)
scale(poolsurface, poolScale)
poolsurface:translate(0.0, poolHeight, 0.0)
poolsurface:set_material(diffuse)
poolsurface:add_texture('felt.bmp')

-- Felt Edges ===========================================

feltEdgeWidth = 3.0 -- width of felt edges around poolsurface
feltLongEdgeScale =
    {feltEdgeWidth, edgeHeight, poolScale[3] + feltEdgeWidth * 2.0}
feltShortEdgeScale =
    {poolScale[1], edgeHeight, feltEdgeWidth}
feltLongEdgeMoveRight =
    (poolScale[1] + feltEdgeWidth) / 2.0
feltLongEdgeMoveForward =
    (poolScale[3] + feltEdgeWidth) / 2.0

leftFeltEdge = gr.mesh('widepatterncube', 'leftFeltEdge')
rootnode:add_child(leftFeltEdge)
scale(leftFeltEdge, feltLongEdgeScale)
leftFeltEdge:translate(
    - feltLongEdgeMoveRight,
    edgeMoveUp,
    0.0)
leftFeltEdge:set_material(diffuse)
leftFeltEdge:add_texture('felt.bmp')

rightFeltEdge = gr.mesh('widepatterncube', 'rightFeltEdge')
rootnode:add_child(rightFeltEdge)
scale(rightFeltEdge, feltLongEdgeScale)
rightFeltEdge:translate(
    feltLongEdgeMoveRight,
    edgeMoveUp,
    0.0)
rightFeltEdge:set_material(diffuse)
rightFeltEdge:add_texture('felt.bmp')

frontFeltEdge = gr.mesh('longpatterncube', 'frontFeltEdge')
rootnode:add_child(frontFeltEdge)
scale(frontFeltEdge, feltShortEdgeScale)
frontFeltEdge:translate(
    0.0,
    edgeMoveUp,
    feltLongEdgeMoveForward)
frontFeltEdge:set_material(diffuse)
frontFeltEdge:add_texture('felt.bmp')

backFeltEdge = gr.mesh('longpatterncube', 'backFeltEdge')
rootnode:add_child(backFeltEdge)
scale(backFeltEdge, feltShortEdgeScale)
backFeltEdge:translate(
    0.0,
    edgeMoveUp,
    - feltLongEdgeMoveForward)
backFeltEdge:set_material(diffuse)
backFeltEdge:add_texture('felt.bmp')

-- END (Felt Edges) ===========================================

-- Wood Edges =================================================

woodEdgeWidth = 4.0 -- width of wood edges around felt edges
woodLongEdgeScale =
    {woodEdgeWidth,
    edgeHeight,
    poolScale[3] + (feltEdgeWidth + woodEdgeWidth) * 2.0}
woodShortEdgeScale =
    {poolScale[1], edgeHeight, feltEdgeWidth}
woodLongEdgeMoveRight =
    (poolScale[1] + woodEdgeWidth) / 2.0 + feltEdgeWidth
woodLongEdgeMoveForward =
    (poolScale[3] + woodEdgeWidth) / 2.0 + feltEdgeWidth

leftWoodEdge = gr.mesh('widepatterncube', 'leftWoodEdge')
rootnode:add_child(leftWoodEdge)
scale(leftWoodEdge, woodLongEdgeScale)
leftWoodEdge:translate(
    - woodLongEdgeMoveRight,
    edgeMoveUp,
    0.0)
leftWoodEdge:set_material(glossy)
leftWoodEdge:add_texture('cherryturned.bmp')

rightWoodEdge = gr.mesh('widepatterncube', 'rightWoodEdge')
rootnode:add_child(rightWoodEdge)
scale(rightWoodEdge, woodLongEdgeScale)
rightWoodEdge:translate(
    woodLongEdgeMoveRight,
    edgeMoveUp,
    0.0)
rightWoodEdge:set_material(glossy)
rightWoodEdge:add_texture('cherryturned.bmp')

frontWoodEdge = gr.mesh('longpatterncube', 'frontWoodEdge')
rootnode:add_child(frontWoodEdge)
scale(frontWoodEdge,
    {poolScale[1] + feltEdgeWidth * 2.0,
    edgeHeight,
    woodEdgeWidth})
frontWoodEdge:translate(
    0.0,
    edgeMoveUp,
    woodLongEdgeMoveForward)
frontWoodEdge:set_material(glossy)
frontWoodEdge:add_texture('cherry.bmp')

backWoodEdge = gr.mesh('longpatterncube', 'backWoodEdge')
rootnode:add_child(backWoodEdge)
scale(backWoodEdge,
    {poolScale[1] + feltEdgeWidth * 2.0,
    edgeHeight,
    woodEdgeWidth})
backWoodEdge:translate(
    0.0,
    edgeMoveUp,
    - woodLongEdgeMoveForward)
backWoodEdge:set_material(glossy)
backWoodEdge:add_texture('cherry.bmp')

-- END (Wood Edges) ===========================================

-- Table Base =================================================

baseWallWidth = feltEdgeWidth -- width of felt edges around poolsurface
baseWallHeight = poolHeight + poolScale[2] / 2.0
baseLongWallScale = feltLongEdgeScale
baseLongWallScale[2] = baseWallHeight
baseShortWallScale = feltShortEdgeScale
baseShortWallScale[2] = baseWallHeight
baseLongWallMoveRight = feltLongEdgeMoveRight
baseLongWallMoveForward = feltLongEdgeMoveForward
baseWallMoveUp = baseWallHeight / 2.0 

leftBaseWall = gr.mesh('longpatterncube', 'leftBaseWall')
rootnode:add_child(leftBaseWall)
scale(leftBaseWall, baseLongWallScale)
leftBaseWall:translate(
    - baseLongWallMoveRight,
    baseWallMoveUp,
    0.0)
leftBaseWall:set_material(glossy)
leftBaseWall:add_texture('cherry.bmp')

rightBaseWall = gr.mesh('longpatterncube', 'rightBaseWall')
rootnode:add_child(rightBaseWall)
scale(rightBaseWall, baseLongWallScale)
rightBaseWall:translate(
    baseLongWallMoveRight,
    baseWallMoveUp,
    0.0)
rightBaseWall:set_material(glossy)
rightBaseWall:add_texture('cherry.bmp')

frontBaseWall = gr.mesh('longpatterncube', 'frontBaseWall')
rootnode:add_child(frontBaseWall)
scale(frontBaseWall, baseShortWallScale)
frontBaseWall:translate(
    0.0,
    baseWallMoveUp,
    baseLongWallMoveForward)
frontBaseWall:set_material(glossy)
frontBaseWall:add_texture('cherry.bmp')

backBaseWall = gr.mesh('longpatterncube', 'backBaseWall')
rootnode:add_child(backBaseWall)
scale(backBaseWall, baseShortWallScale)
backBaseWall:translate(
    0.0,
    baseWallMoveUp,
    - baseLongWallMoveForward)
backBaseWall:set_material(glossy)
backBaseWall:add_texture('cherry.bmp')

-- END (Wood Edges) ===========================================

-- END (Pool Table) ========================================================



-- Balls ======================================================================

ballHeight = poolHeight + poolScale[2] / 2.0 + 1.0

cueball = gr.mesh('sphere', 'cueBall')
rootnode:add_child(cueball)
cueball:translate(0.0, ballHeight, 20.0)
cueball:set_material(glossywhite)

blueball = gr.mesh('sphere', 'blueBall')
rootnode:add_child(blueball)
blueball:translate(0.0, ballHeight, -15.0)
blueball:set_material(glossyblue)

redball = gr.mesh('sphere', 'redBall')
rootnode:add_child(redball)
redball:translate(-1.1, ballHeight, -15.0 - 2.1)
redball:set_material(glossyred)

greenball = gr.mesh('sphere', 'greenBall')
rootnode:add_child(greenball)
greenball:translate(1.1, ballHeight, -15.0 - 2.1)
greenball:set_material(glossygreen)

purpleball = gr.mesh('sphere', 'purpleBall')
rootnode:add_child(purpleball)
purpleball:translate(-1.1 * 2.0, ballHeight, -15.0 - 2.1 * 2.0)
purpleball:set_material(glossypurple)

blackball = gr.mesh('sphere', 'blackBall')
rootnode:add_child(blackball)
blackball:translate(0.0, ballHeight, -15.0 - 2.1 * 2.0)
blackball:set_material(glossyblack)

yellowball = gr.mesh('sphere', 'yellowBall')
rootnode:add_child(yellowball)
yellowball:translate(1.1 * 2.0, ballHeight, -15.0 - 2.1 * 2.0)
yellowball:set_material(glossyyellow)

-- END (Balls) ================================================================



-- Walls ===================================================

nearWallDistance = - 160.0
farWallDistance = - 200.0
ceilingDistance = - 200.0

floorCeilingScale = {-nearWallDistance * 2.0, -farWallDistance * 2.0, 1.0}
nearWallScale = {-ceilingDistance * 2.0, -farWallDistance * 2.0, 1.0}
farWallScale = {-nearWallDistance * 2.0, -ceilingDistance * 2.0, 1.0}

frontWall = gr.mesh('patterncube', 'frontWall')
scale(frontWall, {400.0, 400.0, 1.0})
rootnode:add_child(frontWall)
frontWall:translate(0.0, 0.0, farWallDistance - farWallScale[3] / 2.0)
frontWall:set_material(diffuse)
frontWall:add_texture('brick.bmp')

leftWall = gr.mesh('patterncube', 'leftWall')
scale(leftWall, {400.0, 400.0, 1.0})
rootnode:add_child(leftWall)
leftWall:translate(0.0, 0.0, nearWallDistance - nearWallScale[3] / 2.0)
leftWall:rotate('y', 90.0)
leftWall:set_material(diffuse)
leftWall:add_texture('brick.bmp')

rightWall = gr.mesh('patterncube', 'rightWall')
scale(rightWall, {400.0, 400.0, 1.0})
rootnode:add_child(rightWall)
rightWall:translate(0.0, 0.0, nearWallDistance - nearWallScale[3] / 2.0)
rightWall:rotate('y', -90.0)
rightWall:set_material(diffuse)
rightWall:add_texture('brick.bmp')

backWall = gr.mesh('patterncube', 'backWall')
scale(backWall, {400.0, 400.0, 1.0})
rootnode:add_child(backWall)
backWall:translate(0.0, 0.0, farWallDistance - farWallScale[3] / 2.0)
backWall:rotate('y', 180.0)
backWall:set_material(diffuse)
backWall:add_texture('brick.bmp')

floor = gr.mesh('thickpatterncube', 'floor')
scale(floor, {400.0, 400.0, 1.0})
rootnode:add_child(floor)
floor:translate(0.0, 0.0, - floorCeilingScale[3] / 2.0)
floor:rotate('x', -90.0)
floor:set_material(diffuse)
floor:add_texture('tile.bmp')

ceiling = gr.mesh('cube', 'ceiling')
scale(ceiling, {400.0, 400.0, 1.0})
rootnode:add_child(ceiling)
ceiling:translate(0.0, 0.0, ceilingDistance - floorCeilingScale[3] / 2.0)
ceiling:rotate('x', 90.0)
ceiling:set_material(white)

--============================================================
-- END Walls

-- Puppet Functions
--============================================================

function getMesh(meshId, meshName, meshScale, meshMaterial,    
    parentNode, parentScale)
  --[[
  Get a Mesh whose scaling is independent of its parent's scaling;
  not translated from parent
  meshScale: list of size 3: (x, y, z) scaling
  parentScale: list of size 3: (x, y, z) scaling of parent
  ]]
  newMesh = gr.mesh(meshId, meshName)
  parentNode:add_child(newMesh)
  unscale(newMesh, parentScale)
  scale(newMesh, meshScale)
  newMesh:set_material(meshMaterial)
  return newMesh
end

function getMeshJoint(meshId, meshName, meshScale, meshMaterial,    
    jointNode)
  --[[
  Get a Mesh whose parent is a joint; the Mesh will be translated
  such that it's (top-most) endpoint is incident with the joint
  meshScale: list of size 3: (x, y, z) scaling
             (w.r.t. parent's scale)
  jointNode: parent joint node
  ]]
  newMesh = gr.mesh(meshId, meshName)
  jointNode:add_child(newMesh)
  scale(newMesh, meshScale)
    -- position endpoint at the joint
  newMesh:translate(0, -meshScale[2]/2.0, 0)
  newMesh:set_material(meshMaterial)
  return newMesh
end

function arm(name_prefix, rotation, parentNode, parentScale)
  --[[
  Returns an arm rooted at the shoulder joint; removes parent scaling
  name_prefix: string to prefix onto node names
  rotation: list of size 3: (x, y, z) rotation to apply to leg
  parentNode: node of parent
  parentScale: list of size 3: (x, y, z) scale of parent
  ]]
  xRange = {-80, 0, 40}
  if name_prefix == 'right' then
    yRange = {-40, 0, 40}
  else -- left
    yRange = {-40, 0, 40}
  end
  shoulderJoint = gr.joint(name_prefix .. 'ShoulderJoint',
      xRange, yRange)
  rotate(shoulderJoint, rotation)
  parentNode:add_child(shoulderJoint)
  unscale(shoulderJoint, parentScale)

  -- shoulder
  shoulderScale = {0.09, 0.09, 0.09}
  shoulder = gr.mesh('sphere', name_prefix .. 'Shoulder')
  shoulderJoint:add_child(shoulder)
  scale(shoulder, shoulderScale)
  shoulder:set_material(chrome)

  -- upperarm
  upperarmScale = multEach({0.12, 0.5, 0.12}, 0.5)
  upperarm = getMeshJoint('sphere', name_prefix .. 'Upperarm',
      upperarmScale, chrome, shoulderJoint)
  upperarm:translate(0, -0.06, 0)

  -- elbow joint
  xRange = {-90, 0, 0}
  yRange = {-80, 0, 80}
  elbowJoint = gr.joint(name_prefix .. 'ElbowJoint', 
      xRange, yRange)
  upperarm:add_child(elbowJoint)
  unscale(elbowJoint, upperarmScale)
  elbowJoint:translate(0, -0.74, 0)

  -- elbow
  elbowScale = {0.06, 0.06, 0.06}
  elbow = gr.mesh('sphere', name_prefix .. 'Elbow')
  elbowJoint:add_child(elbow)
  scale(elbow, elbowScale)
  elbow:set_material(red)

  -- lowerarm
  lowerarmScale = multEach({0.1, 0.45, 0.1}, 0.5)
  lowerarm = getMeshJoint('sphere', name_prefix .. 'Lowerarm',
      lowerarmScale, red, elbowJoint)
  lowerarm:translate(0, -0.07, 0)

  -- wrist joint
  xRange = {-90, 0, 90}
  if name_prefix == 'right' then
    yRange = {-25, 0, 25}
  else
    yRange = {-25, 0, 25}
  end
  wristJoint = gr.joint(name_prefix .. 'WristJoint',
      xRange, yRange)
  lowerarm:add_child(wristJoint)
  --rotate(ankleJoint, {-90, 0, 0})
  unscale(wristJoint, lowerarmScale)
  wristJoint:translate(0, -0.9, 0)

  -- wrist
  wristScale = {0.04, 0.04, 0.04}
  wrist = gr.mesh('sphere', name_prefix .. 'Wrist')
  wristJoint:add_child(wrist)
  scale(wrist, wristScale)
  wrist:set_material(red)

  -- hand
  handScale = multEach({0.11, 0.25, 0.04}, 0.4)
  hand = getMeshJoint('sphere', name_prefix .. 'Hand',
      handScale, red, wristJoint)
  hand:translate(0, -0.03, 0)
  
  return shoulderJoint
end

function leg(name_prefix, rotation, parentNode, parentScale)
  --[[
  Returns a leg rooted at the hip joint; removes parent scaling
  name_prefix: string to prefix onto node names
  rotation: list of size 3: (x, y, z) rotation to apply to leg
  parentNode: node of parent
  parentScale: list of size 3: (x, y, z) scale of parent
  ]]
  xRange = {-90, 0, 30}
  if name_prefix == 'right' then
    yRange = {0, 0, 0}
  else -- left
    yRange = {0, 0, 0}
  end
  hipJoint = gr.joint(name_prefix .. 'HipJoint',
      xRange, yRange)
  rotate(hipJoint, rotation)
  parentNode:add_child(hipJoint)
  unscale(hipJoint, parentScale)

  -- hip
  hipScale = {0.12, 0.12, 0.12}
  hip = gr.mesh('sphere', name_prefix .. 'Hip')
  hipJoint:add_child(hip)
  scale(hip, hipScale)
  hip:set_material(chrome)

  -- upperleg
  upperlegScale = multEach({0.12, 0.5, 0.12}, 0.7)
  upperleg = getMeshJoint('sphere', name_prefix .. 'Upperleg',
      upperlegScale, chrome, hipJoint)
  upperleg:translate(0, -0.1, 0)

  -- knee joint
  xRange = {0, 0, 130}
  yRange = {-5, 0, 5}
  kneeJoint = gr.joint(name_prefix .. 'KneeJoint', 
      xRange, yRange)
  upperleg:add_child(kneeJoint)
  unscale(kneeJoint, upperlegScale)
  kneeJoint:translate(0, -0.8, 0)
  
  -- knee
  kneeScale = {0.08, 0.08, 0.08}
  knee = gr.mesh('sphere', name_prefix .. 'Knee')
  kneeJoint:add_child(knee)
  scale(knee, kneeScale)
  knee:set_material(red)

  -- lowerleg
  lowerlegScale = multEach({0.1, 0.5, 0.1}, 0.7)
  lowerleg = getMeshJoint('sphere', name_prefix .. 'Lowerleg',
      lowerlegScale, red, kneeJoint)
  lowerleg:translate(0, -0.1, 0)

  -- ankle joint
  xRange = {-20, 0, 40}
  if name_prefix == 'right' then
    yRange = {-5, 0, 10}
  else
    yRange = {-10, 0, 5}
  end
  ankleJoint = gr.joint(name_prefix .. 'AnkleJoint',
      xRange, yRange)
  lowerleg:add_child(ankleJoint)
  rotate(ankleJoint, {-90, 0, 0})
  unscale(ankleJoint, lowerlegScale)
  ankleJoint:translate(0, -0.8, 0)

  -- ankle
  ankleScale = {0.07, 0.07, 0.07}
  ankle = gr.mesh('sphere', name_prefix .. 'Ankle')
  ankleJoint:add_child(ankle)
  scale(ankle, ankleScale)
  ankle:set_material(red)

  -- foot
  footScale = multEach({0.11, 0.25, 0.04}, 0.7)
  foot = getMeshJoint('sphere', name_prefix .. 'Foot',
      footScale, red, ankleJoint)
  foot:translate(0, -0.1, -0.02)
  
  return hipJoint
end
-- END puppet functions ========================================================

-- Puppet ======================================================================

puppetScaleValue = 30.0
puppetScale = {puppetScaleValue, puppetScaleValue, puppetScaleValue}

-- torso
torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torsoScale = multEach({0.5, 0.9, 0.3}, 0.6)
scale(torso, torsoScale)
scale(torso, puppetScale)
torso:rotate('y', 35.0)
torso:translate(-130.0, 1.7 * puppetScaleValue, -170.0)
torso:set_material(chrome)

-- waist joint
waistjoint = gr.joint('waistJoint', {0, 0, 0}, {-20, 0, 20})
torso:add_child(waistjoint)
unscale(waistjoint, torsoScale) -- remove parent scaling
waistjoint:translate(0, -0.6, 0)

-- pelvis
pelvisScale = multEach(
    {torsoScale[1] * 0.9 * 1.5,
    torsoScale[2] * 0.3 * 1.5,
    torsoScale[3] * 0.8 * 1.5},
    0.7)
pelvis = getMeshJoint('sphere', 'pelvis', pelvisScale, chrome,    
    waistjoint)

-- chest
chestScale = multEach({0.75, 0.4, 0.35}, 0.4)
chest = getMesh('sphere', 'chest', chestScale, chrome,    
    torso, torsoScale)
chest:translate(0, 6/8, 0) -- move to top of torso

-- left leg
left_leg = leg('left', {0, 0, 0}, pelvis, pelvisScale)
left_leg:translate(-5/9, -6/8, 0) 

-- right leg
right_leg = leg('right', {0, 0, 0}, pelvis, pelvisScale)
right_leg:translate(5/9, -6/8, 0) -- translate w.r.t pelvis scale

-- left arm
left_arm = arm('left', {0, 0, -45}, chest, chestScale)
left_arm:translate(-1, -0.15, 0) 

-- right arm
right_arm = arm('right', {0, 0, 45}, chest, chestScale)
right_arm:translate(1, -0.15, 0) 

-- base of neck
neckbase = gr.joint('neckbase', {0, 0, 0}, {-80, 0, 80})
chest:add_child(neckbase)
unscale(neckbase, chestScale) -- remove parent scaling
-- neckbase position is incident with top of torso
neckbase:rotate('x', -180)
neckbase:translate(0, 1/2, 0)

-- neck
neckScale = multEach({0.1, 0.3, 0.1}, 0.7)
neck = getMeshJoint('sphere', 'neck', neckScale, chrome,    
    neckbase)
neck:translate(0, 0, 0)

-- base of head
headbase = gr.joint('headbase', {-35, 0, 45}, {0, 0, 0})
neck:add_child(headbase)
unscale(headbase, neckScale) -- remove parent scaling
headbase:translate(0, -1/8, 0)

-- head
headScale = {0.4, 0.3, 0.4}
head = getMeshJoint('cube', 'head', headScale, chrome,    
    headbase)

-- antenna
antennaScale = {1.5, 0.08, 0.08}
antenna = gr.mesh('sphere', 'antenna')
head:add_child(antenna)
scale(antenna, antennaScale)
antenna:rotate('z', -90)
antenna:set_material(chrome)

balltip = gr.mesh('sphere', 'balltip')
antenna:add_child(balltip)
unscale(balltip, antennaScale)
balltip:scale(0.2, 0.2, 0.2)
balltip:translate(0.9, 0, 0)
balltip:set_material(red)

-- left eye
leftEye = gr.mesh('cube', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.2, 0.1, 0.1)
leftEye:translate(0.2, -0.2, -0.5)
leftEye:set_material(yellow)

-- right eye
rightEye = gr.mesh('cube', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.2, 0.1, 0.1)
rightEye:translate(-0.2, -0.2, -0.5)
rightEye:set_material(yellow)

-- END puppet ==================================================================

return rootnode
