Final assignment of the Games Concepts module in year 1 (2018/2019). Made with TL Engine and C++. Achieved 120 points out of 100 if I recall correctly. 8)

Features:
  Hover car movement (thrust and drag, tilting and bobbing)
  Collision detection (spheres and boxes)
  Health points (lost on collision, near explosions and when on fire)
  Huge race track loaded from file (made with a slapdash level maker, not included because the code was a mess)
  UI displaying race and player car status, equipped with a visual boost bar 
  3 "AI" opponents (following one of two lanes and switching between them, variable speed and health)
  Particle systems (fire/exhaust, explosion and smoke)
  Cool textures

Controls:
  WASD - move
  Space - boost
  Arrows - move camera
  123 - switch between camera modes/reset camera position and orientation
  
If the project doesn't start, it can be easily recreated by making a new project with TL Engine (http://gamesnorthwest.net/resources/tl_engine/downloads.html), replacing the cpp file's contents and putting the media folder and the level.txt file inside that new project. The project solution might also need to be retargetted to version 10.0.17134.0 of the Windows SDK.
