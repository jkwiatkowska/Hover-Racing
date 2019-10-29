//Justyna Kwiatkowska G20714950

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <cmath>
#include <fstream> //Files
#include <vector> //Dynamic arrays
#include <sstream> //Joining and storing text
#include <iomanip> //Leading zeros
#include <algorithm> //Vector shuffle

using namespace tle;
using namespace std;

struct Time
{
	int m;
	int s;
	int ms;
};

//Given a number of seconds return time in hours, minutes and seconds
Time GetTime(float seconds);

struct Vector3D
{
	float x;
	float y;
	float z;

	float Length()
	{
		return pow(x, 2) + pow(y, 2) + pow(z, 2);
	}

	Vector3D Normal()
	{
		return { x / sqrt(Length()), y / sqrt(Length()), z / sqrt(Length()) };
	}

	Vector3D operator + (Vector3D v2) //Addition
	{
		return { x + v2.x, y + v2.y, z + v2.z };
	}

	Vector3D operator - (Vector3D v2) //Subtraction
	{
		return { x - v2.x, y - v2.y, z - v2.z };
	}

	Vector3D operator * (Vector3D v2) //Multiplication
	{
		return { x * v2.x, y * v2.y, z * v2.z };
	}

	Vector3D operator * (float scale) //Scalar multiplication
	{
		return { x * scale, y * scale, z * scale };
	}
};

struct Vector2D
{
	float x;
	float z;

	float Length(); //Length of a vector without sqrt applied
	Vector2D Normal(); //Normalised vector

	//Operations
	Vector2D operator + (Vector2D v2);
	Vector2D operator - (Vector2D v2);
	Vector2D operator - ();
	Vector2D operator * (float scale);
	Vector2D operator * (Vector2D v2);

	//Comparisons
	bool operator < (Vector2D v2);
	bool operator < (float f);
	bool operator > (Vector2D v2);
	bool operator > (float f);
	bool operator == (Vector2D v2);
};

Vector2D FacingVector(IModel* m); //Calculate the facing vector or a model

//General constants
const Vector3D kGravity = { 0.0f, -50.0f, 0.0f };
const float kPi = 3.1415926f;
const float kMpsToKmph = 3.6f;

//Game constants
const float kScale = 1.3f; //Metres per units
const int kLaps = 2; //Total number of laps
const int kMaxCars = 4; //Maximum number of cars in game
const int kMaxHP = 100; //Full health of a hover car
const float kLowHP = 0.3f; //Percentage of health where boost stops working
const float kUpPerSec = 0.2f; //Updates per second, applied to the speed display
const float kMaxCount = 3.0f; //Starting value when counting down
const float kBoostTime = 3.0f; //Max time of boost before overheat
const int kLaneNumber = 2; //Number of lanes that the AIs can choose from

//Grid constants
const int kGridSize = 40;
const int kTerrainSize = 2000;
const int kGridSquares = kTerrainSize / kGridSize;

/****Media****/
const string kMediaFolder = ".\\Media";

const string kLevelFile = "level.txt";

//Scenery
const string kMeshSky = "Skybox 07.x";
const string kMeshGround = "ground.x";
const string kMeshHills = "Hills.x";
const string kMeshBush = "Bush.x";
const string kMeshWalkway = "Walkway.x";

//Functional
const string kMeshDummy = "Dummy.x";
const string kMeshCar = "race2.x"; //Player car
const string kMeshCheckpoint = "Checkpoint.x";
const string kMeshCross = "Cross.x";
const string kMeshBomb = "Flare.x";

const string kSkinPlayer = "sp02-01-blue.jpg";
const string kSkinAI = "sp02-01-pink.jpg";

//Obstacles
const string kMeshTank1 = "TankSmall1.x";
const string kMeshTank2 = "TankSmall2.x";
const string kMeshIsle = "IsleStraight.x";
const string kMeshIsle2 = "IsleDark.x";
const string kMeshWall = "Wall.x";
const string kMeshTribune = "Tribune1.x";
const string kMeshSkyscraper = "skyscraper02.x";
const string kMeshSkyscraper2 = "skyscraper13.x";
const string kMeshBuilding = "Building03.x";

//Keys
const EKeyCode kKeyQuit = Key_Escape;
const EKeyCode kKeyRestart = Key_F1;

const EKeyCode kKeyCameraChase = Key_1;
const EKeyCode kKeyCameraFP = Key_2;
const EKeyCode kKeyCameraFree = Key_3;
const EKeyCode kKeyCameraUp = Key_Up;
const EKeyCode kKeyCameraDown = Key_Down;
const EKeyCode kKeyCameraRight = Key_Right;
const EKeyCode kKeyCameraLeft = Key_Left;
const EKeyCode kKeyCameraCapture = Key_Tab;

const EKeyCode kKeyCarForward = Key_W;
const EKeyCode kKeyCarBackward = Key_S;
const EKeyCode kKeyCarRight = Key_D;
const EKeyCode kKeyCarLeft = Key_A;

const EKeyCode kKeyStart = Key_Space;
const EKeyCode kKeyBoost = Key_Space;

//Size/position constants
const Vector2D kWindowSize{ 1280.0f, 720.0f };

const Vector3D kPosSky = { 0.0f, -960.0f, 0.0f };

const float kStartPosDistance = -9.0f; //Distance of the starting line from the first checkpoint
const float kStartPositions[kMaxCars]{ -5.2f, -1.9f, 1.9f, 5.2f }; //Positions of each car in the beginning, from the first checkpoint's center

const float kIsleWid = 1.8f; //Half the width of an isle
const float kIsleLen = 2.5f; //Half the length of an isle
const float kWallWid = 0.9f; //Half the width of a wall
const float kWallLen = 4.8f; //Half the length of a wall
const float kCheckpointRad = 1.3f; //Struct radius
const float kCheckpointLen = 9.7f; //Length from the middle of a checkpoint to each struct
const float kCheckpointWidth = 1.0f; //Width of the collision box that determines whether the checkpoint was crossed
const float kWorldLen = 999.0f; //Distance of world edges from origin

const float kTankScale = 0.4f;
const float kTank2Y = -1.2f;
const float kTank2Rot = 10.0f;
const float kTribuneScale = 0.6f;
const float kSkyscraperScale = 0.4f;
const float kSkyscraper2Scale = 0.22f;
const float kBuildingScale = 0.5f;
const float kBushScale[4]{ 6.0f, 10.0f, 15.0f, 20.0f };
const float kHillScale = 4.5f;
const float kHillY = -18.0f;
const float kWalkwayScale = 4.0f;

const float kTankRad = 1.8f * kTankScale;
const float kTribuneRad = 4.0f * kTribuneScale;
const float kBuildingWidth = 10.0f; //Width of half the main section of the building
const float kBuildingWidh2 = 3.5f; //Half width of one of the edge sections
const float kSkyscraperLength1 = 11.0f; //Bigger part's half length
const float kSkyscraperLength2 = 14.5f; //Smaller part's half length
const float kSkyscraperWidth1 = 7.0f; //Bigger part's half width
const float kSkyscraperWidth2 = 1.5f; //Smaller part's half width
const float kSkyscraperAdjustment = -2.0f; //Distance of the building's middle from the model origin
const float kSkyscraper2Length = 8.2f; //Rectangle part's half length
const float kSkyscraper2Width = 3.55f; //Width
const float kSkyscraper2Radius = 4.2f; //Round part's radius

const float kTankFireHeight = 1.9f;
const float kTankFireRad = 2.0f;

enum ParticleType { fire, smoke, exhaust, explosion };
//Particles
struct Particle
{
	IModel* particle;

	Vector3D origin; //Starting position
	float radius; //Radius of the origin area

	Vector3D v; //Velocity
	Vector3D sv; //Starting velocity

	ParticleType type; //Type of the particle
	float totalLife; //Total time the particle lives
	float maxLife; //Max lifetime possible
	float minLife; //Min lifetime possible
	float life = 0.0f; //Time passed since creation
	float minVel; //Respawn if velocity becomes shorter than this
	float angle;

	//Functions
	void CreateParticle(IMesh* particleMesh, string skin, Vector3D particleOrigin, float originRadius, Vector3D startVelocity, float minLifetime, float maxLifetime, float minVelocity,
		ParticleType pType, float pAngle = 0.0f); //Create a new particle model and set its skin and variables

	float RandomAngle(float angle); //Generate a random angle within a specified range
	void Spawn(Vector2D momentum = { 0.0f, 0.0f }); //Reset a particle's position and velocity

	void Update(float fTime, Vector3D acceleration, Vector3D partOrigin, ICamera* *camera, bool isActive, Vector2D momentum = { 0.0f, 0.0f }, Vector3D newStartingVelocity = { 0.0f, 0.0f, 0.0f });
	//Update orientation and velocity, move and respawn after death
};

struct ExplosionEmitter
{
	const vector<string> explosionSkin{ "Explosion1.jpg", "Explosion2.jpg", "Explosion3.jpg", "Explosion4.jpg", "Explosion5.jpg" };

	static const int kMaxParticles = 17;
	const float kFrequency = 0.006f; //Frequency at which new particles are spawned

	const float kStartSpeed = 9.2f; //Starting speed, multiplied by a random direction vetor and used  when a particle is spawned
	const float kAcceleration = -0.5f; //Acceleration ratio
	const float kMinVel = 0.1f; //If velocity becomes lower than this, respawn/kill the particle

	const float kMinLife = 0.7f; //Minimum life of each particle
	const float kMaxLife = 1.6f; //Maximum life of each particle

	const float kParticleHeight = 1.5f; //Y position of the particle origin

	IMesh* eMesh;
	Particle particle[kMaxParticles];

	Vector3D sVelocity = { 0.0f, 0.0f, 0.0f }; //Starting velocity
	float radius; //Radius of the emitter 
	Vector3D origin;
	float velRatio; //Velocity can be lowered or increased with this

	int particleIndex = 0; //Next particle to be emitted

	float timer = 0.0f; //Counts the time between particle spawns

	ExplosionEmitter(IMesh* explosionMesh, Vector3D emitterOrigin, float explosionRadius = 0.1f, float velocityRatio = 1.0f); //Constructor
	void NewParticle(); //Add a new particle to to the array of particles
	Vector3D NewSV(); //Generate a random starting vector so that the explosion particles can shoot in any direction

	void Update(float fTime, ICamera* *camera, bool isActive = 1, Vector2D momentum = { 0, 0 }); //Spawn more particles and update the existing ones
	void UpdateOrigin(Vector3D particleOrigin); //Change origin position if it had moved
};

struct SmokeEmitter
{
	const vector<string> smokeSkin{ "Smoke1.jpg", "Smoke2.jpg", "Smoke3.jpg", "Smoke4.jpg", "Smoke5.jpg", "Smoke6.jpg" };

	static const int kMaxParticles = 10;
	const float kFrequency = 0.2f; //Frequency at which new particles are spawned

	const Vector3D sVelocity = { 0.0f, 6.0f, 0.0f }; //Starting velocity
	const Vector3D kAcceleration = { 0.0f, 5.0f, 0.0f }; //Acceleration per second per second
	const float kMinVel = 0.0f * sVelocity.y; //If velocity becomes lower than this, respawn the particle
	const float kAngle = 4.0f; //Maximum angle of the emission

	const float kMinLife = 1.2f; //Minimum life of each particle
	const float kMaxLife = 1.8f; //Maximum life of each particle


	IMesh* sMesh;
	Particle particle[kMaxParticles];

	float radius; //Radius of the emitter 
	Vector3D origin;
	float velRatio; //Velocity can be lowered or increased with this

	int particleIndex = 0; //Next particle to be emitted

	float timer = 0.0f; //Counts the time between particle spawns

	SmokeEmitter(IMesh* fireMesh, Vector3D emitterOrigin, float smokeRadius = 2.5f, float velocityRatio = 1.0f); //Constructor
	void NewParticle(); //Add a new particle to to the array of particles

	void Update(float fTime, ICamera* *camera, bool isActive = 1, Vector2D momentum = { 0, 0 }); //Spawn more particles and update the existing ones
	void UpdateOrigin(Vector3D particleOrigin); //Change origin position if it had moved
};

struct FireEmitter
{
	//Fire particles
	const vector<string> fireSkin{ "Fire1.jpg", "Fire2.jpg", "Fire3.jpg", "Fire4.jpg", "Fire5.jpg", "Fire6.jpg", "Fire7.jpg", "Fire8.jpg", "Fire11.jpg" };

	static const int kMaxParticles = 25;
	const float kFrequency = 0.01f; //Frequency at which new particles are spawned

	const Vector3D sVelocity = { 0.0f, 40.0f, 0.0f }; //Starting velocity of new particles
	const float kMinVel = 0.8f * sVelocity.y; //If velocity becomes lower than this, respawn the particle

	const float kMinLife = 0.01f; //Minimum life of each particle
	const float kMaxLife = 0.1f; //Maximum life of each particle

	//Fire2 particles
	const vector<string> fire2Skin{ "Fire9.jpg", "Fire10.jpg" };

	static const int kMaxParticles2 = 5;
	const float kFrequency2 = 0.03f; //Frequency at which new particles are spawned

	const Vector3D sVelocity2 = { 0.0f, 33.0f, 0.0f }; //Starting velocity of new particles
	const float kMinVel2 = 0.05f * sVelocity.y; //If velocity becomes lower than this, respawn the particle

	const float kMinLife2 = 0.01f; //Minimum life of each particle
	const float kMaxLife2 = 0.4f; //Maximum life of each particle


	//Particles
	IMesh* fMesh;
	Particle particle[kMaxParticles];
	Particle particle2[kMaxParticles2];

	float radius; //Radius of the emitter 
	Vector3D origin;
	float velRatio; //Velocity can be lowered or increased with this

	int particleIndex = 0; //Next particle to be emitted
	int particleIndex2 = 0; //Next type 2 particle to be emitted

	float timer = 0.0f; //Counts the time between particle spawns
	float timer2 = 0.0f; //Counts the time between particle spawns of the second type

	FireEmitter(IMesh* fireMesh, Vector3D emitterOrigin, float fireRadius = 2.5f, float velocityRatio = 1.0f); //Constructor
	void NewParticle(); //Add a new particle to to the array of particles
	void NewParticle2(); //Add a new particle to to the second array of particles


	void Update(float fTime, ICamera* *camera, bool isActive = 1, Vector2D momentum = { 0.0f, 0.0f }); //Spawn more particles and update the existing ones
	void UpdateOrigin(Vector3D particleOrigin); //Change origin position if it had moved
};

struct ExhaustEmitter
{
	const vector<string> exhaustSkin{ "Fire4.jpg", "Fire7.jpg", "Fire8.jpg", "Fire11.jpg" };

	static const int kMaxParticles = 15;
	const float kFrequency = 0.01f; //Frequency at which new particles are spawned

	const Vector3D sVelocity = { 0.0f, -3.0f, 0.0f }; //Starting velocity
	const float kMinVel = 0.8f * sVelocity.y; //If velocity becomes lower than this, respawn the particle

	const float kMinLife = 0.01f; //Minimum life of each particle
	const float kMaxLife = 0.2f; //Maximum life of each particle

	const float kAngle = 17.0f; //Max angle the particles can shoot from


	IMesh* eMesh;
	Particle particle[kMaxParticles];

	float radius; //Radius of the emitter 
	Vector3D origin; //Location the particles spawn from
	float velRatio; //Velocity can be lowered or increased with this

	int particleIndex = 0; //Next particle to be emitted
	int particleIndex2 = 0; //Next type 2 particle to be emitted

	float timer = 0.0f; //Counts the time between particle spawns
	float timer2 = 0.0f; //Counts the time between particle spawns of the second type

	ExhaustEmitter(IMesh* exhaustMesh, Vector3D emitterOrigin, float exhaustRadius = 0.1f, float velocityRatio = 1.0f); //Constructor
	void NewParticle(); //Add a new particle to to the array of particles

	void Update(float fTime, ICamera* *camera, bool isActive = 1, Vector2D momentum = { 0.0f, 0.0f }); //Spawn more particles and update the existing ones
	void UpdateOrigin(Vector3D particleOrigin); //Change origin position if it had moved
};

//Hover cars
enum ColAxis { colX, colZ, both, none }; //Used to determine how the car bounces off square obstacles
enum Speed { fast, slow }; //AI speed ranges

struct HoverCar
{
	//Size constants
	const float kCarScale = 0.4f;
	const float kCarRadius = 2.8f;

	//Movement constants
	const float kCarRotation = 100.0f; //Car rotation speed (angle per second)

	const Vector2D kZeroVector{ 0.0f, 0.0f }; //Used to make resetting vectors easier
	const float kThrustFactor = 150.0f; //Thrust applied each second (multiplied by direction vector)
	const float kDragCoefficient = -3.1f; //Momentum is multiplied by this number to obtain drag

	const float kBoostMult = 1.4f; //Thrust is multiplied by this when boost is on
	const float kBoostMinThrust = 0.01f; //Minimum thrust length required for boost to activate

	const float kSlow = 12.0f; //When the car slows down to this point after collision it can move again

	const float kCarHoverHeight = 2.6f; //Medium height at which the car hovers
	const float kCarHoverRange = 0.5f; //Variation in hover height (defines min/max height)
	const float kCarHoverSpeed = 1.0f; //Speed at which the car moves up and down

	const float kCarMaxTilt = 15.0f; //Max angle at which the car tilts when accelerating
	const float kCarTiltFactor = 40.0f; //Tilt angle reached in a second
	const float kCarMaxLean = 19.0f; //Max angle at which the car leans when steering
	const float kCarLeanFactor = 45.0f; //Lean angle reached in a second at momentum of length 1
	const float kTiltDrag = 5.0f;

	//Time constants
	const float kOverheatPenalty = -5.0f; //Penalty to the boost timer
	const float kExplosionCooldown = 3.0f; //Explosion damage can only be taken once per 2s

	//Collision and damage constants
	const float kDamageFactor = 0.04f; //Multiplied by the current speed to determine damage taken on collision
	const float kCarColRadiusMult = 2.5f; //During car collision the radius is multiplied by this number. The extra 0.5 is to make collisions look more natural
	const float kCarColImpact = 1.9f; //New momentum is multiplied by this number to push cars away

	const float kBombDamage = 15.0f; //Damage taken from explosion, divided by distance from the bomb
	const float kBombImpact = -280.0f; //Multiplied by distance from bomb and vector between it and car to create a pushback effect
	const float kBombMinDist = 2.3f; //Used to ensure that the explosion impact isn't too strong
	const float kBombMaxDist = 5.0f; //Used to ensure that the explosion impact isn't too weak

	const int kBurnDamage = 1; //Damage inflicted from burns
	const float kBurnDamageInterval = 0.5f; //Intervals between which the car takes burn damage

	//Particle constants
	const float kBurnTime = 5.0f; //Time the car burns for
	const float kBurnRadius = 1.5f; //Size of the flame on the X/Z axis
	const float kBurnVelRatio = 0.45f; //Height of the flame's origin
	const float kBurnHeight = 1.4f; //Placement of the flame on the Y axis
	const float kBurnExtinguishSpeed = 1800.0f; //If momentum length is less than this value fire will disappear faster
	const float kBurnExtinguisMult = 1.8f; //Burn timer goes down this much faster when the car slows down enough

	const float kSmokeHeight = 2.15f; //Distance from the car's centre on the Y axis
	const float kSmokeZPos = -1.4f; //Distance from the car's centre on the Z axis
	const float kSmokeRadius = 0.1f; //Radius of the smoke origin
	const float kSmokeMomentumMult = -0.2f; //Part of the momentum that's applied to smoke velocity

	const float kExhaustHeight = 1.7f; //Distance from the car's centre on the Y axis
	const float kExhaustZPos = -1.3f; //Distance from the car's centre on the Z axis
	const float kExhaustRadius = 0.1f; //Radius of the particle origin
	const float kExhaustMinSpeed = 1900.0f; //Minimum momentum length for the exhaust to activate
	const float kExhaustMinBoost = 1.01f; //Minimum boost multiplier value for the exhaust to activate

	//Engine
	I3DEngine* e;

	//Models
	IModel* dummy; //Basic movements, chase camera
	IModel* car; //Tilting/leaning/bobbling, first person camera

	//Time
	float fTime; //Frame time, used as speed multiplier
	float raceTime = 0; //Counts time since start of the race

	//Movement
	Vector2D momentum = kZeroVector; //Current movement speed and direction
	Vector2D thrust = kZeroVector; //Force applied to momentum, dependent on direcion
	Vector2D drag = kZeroVector; //Backward force based on momentum
	Vector2D fVector; //Facing vector, used to calculate thrust 
	float thMult = 1.0f; //Thrust multiplier (used to block or lower acceleration)
	float boostMult = 1.0f; //Another thrust multiplier, applied when boosting
	float drMult = 1.0f; //Drag multiplier (inceased to slow down)

	enum Bobble { up = 1, down = -1 };
	Bobble bobbleDir = up; //Tracks the direction in which the car is bobbling
	float tilt = 0.0f;
	float lean = 0.0f;

	//Collision detection
	Vector2D currentSquare;

	float r = kCarScale * kCarRadius; //Car radius

	Vector2D prevPos = kZeroVector;
	int colIndexSphere = -1; //Tracks spheres collided with
	int colIndexBox = -1; //Tracks boxes collided with
	int colIndexCar = -1; //Tracks cars collided with

	//Race
	string name; //Name to display if the car wins
	size_t nextCheck = 0; //Next checkpoint that the car has to fly through
	int lap = 1; //Current lap number
	int racePos = 1;

	//Health
	int hp = kMaxHP; //Health points
	int colDamage = 0; //Damage taken on collision, depends on the momentum
	float explosionTimer = 0.0f; //Keeps track of when the last explosion happened

	//Boost
	float boostTimer = kBoostTime; //Goes down when boost is on, up when it isn't
	bool boostLock = 0;

	//Particles
	vector<FireEmitter> fire;
	vector<SmokeEmitter> smoke;
	vector<ExhaustEmitter> exhaust;

	float burnTimer = 0.0f;
	float burnDamageTimer = 0.0f;

	/****Non-player cars****/
	const float kGoalSpeed = 270.0f; //Speed at which the goal dummy moves towards the next waypoint, divided by distance to car
	const float kMaxGoalDist = 8.0f; //The car needs to be this close to the goal dummy for it to move forward

	const float kMinThrust = 0.55f; //Minimum AI thrust multiplier (applied in place of player's boost multiplier)
	const float kMidThrust = 1.15f; //New thrust is selected from values between this and min or max
	const float kMaxThrust = 1.6f; //Maximum AI thrust multiplier (applied in place of player's boost multiplier)
	const float kThrustChange = 1.5f; //Speed at which the AI speed increases and decreases, per second
	const float kSpeedChangeCD = 2.0f; //At least 2 seconds have to pass between two speed changes
	const float kThrustBonus = 0.04f; //Thrust is increased by this times current place in the race

	const float kLowHPPenalty = 0.4f; //Multiplier applied to thrust if hp is at 30%
	const float kMedHPPenalty = 0.9f; //Multiplier applied if hp is at 60%

	bool isAI; //True if the car is not controlled by player
	vector<vector<Vector2D>> path; //Used to store the route to be taken by computer-contolled car 
	size_t lane = 0; //Index in the first dimension of the path vector, determines the set of waypoints that's followed
	size_t currentGoal = 0; //Index in the second dimension of the path vector, determines the next position to be taken
	IModel* goal; //A dummy model that the car automatically follows
	IModel* nextWaypoint; //Dummy model that the goal dummy moves towards;
	float newThrust = 1.0f; //New thrust multiplier for AI to slowly change to
	float speedChangeCD = 0.0f; //Cooldown on speed changes

	/****Functions****/
	HoverCar(IMesh* dummyMesh, IMesh* carMesh, IMesh* particleMesh, vector<vector<Vector2D>> &paths, float startX, float startZ, string carName, int carNo, bool ai = 1); //Constructor

	void Reset(float startX, float startZ); //Reset the car's variables and move it to a given starting position

	void AIFollowPath(); //Update AI orientation and speed, move the goal dummy and change waypoints when needed
	void AINewSpeed(Speed speed); //Switch to a random speed in a slow or fast range
	void AINextWaypoint(); //Switch to next waypoint on AI's path
	void AISwitchLane(); //Switch to a different lane
	void ComparePosition(HoverCar *car2, IModel* &checkpoint); //Compare position in the race with another racer

	void UpdateTime(); //Update race time
	void UpdateDamage(); //Damage related updates
	void UpdateParticles(ICamera* *camera); //Update fire, smoke and exhaust fire particles coming from the car

	void Controls(I3DEngine* e); //Take keyboard input and react accordingly

	void TakeDamage(int damage); //Subtract damage and disable thrust if hp goes too low
	void ResetCollision(); //Return to normal speed and enable collisions when the car slows down enough
	void SphereCollision(int index); //Collision with a sphere shaped obstacle
	void BoxCollision(int index, ColAxis a); //Collision with a box shaped obstacle
	bool CarCollision(HoverCar *car2, int index); //Collision with another car
	void Burn(ICamera* *camera); //Emit fire particles and take damage
	void Explosion(IModel* *bomb); //Push the car away from bomb and take damage

	void Move(); //Move the car according to its momentum
	void Rotate(); //Update car's orientation based on lean and tilt values
	void Bobble(); //Move the car up and down
	void Tilt(float dir); //Update the tilt value, takes a direction multiplier of 1 or -1
	void Lean(float dir); //Update the lean value, takes a direction multiplier of 1 or -1
	void Boost(I3DEngine* e); //Checks performed when player attempts to use boost, along with consecutive actions

	void Update(float frameTime, ICamera* *camera); //Actions performed every frame
};

struct Camera
{
	//Camera settings
	struct CamSetting
	{
		Vector3D pos; //Position of the camera
		int mouseX; //X rotation of the mouse
		int mouseY; //Y rotation of the mouse
	};
	const vector<CamSetting> kCameraModes = { { { 0.5f, 12.0f, -19.0f }, 0, 190 }, { { 0.0f, 4.85f, 1.2f }, 0, -20 }, { { -34.9f, 140.4f, -169.0f }, 269, 381 } };

	//Constants
	const float kCameraSpeed = 40.0f; //Speed of manual camera movement
	const float kCameraRotation = 0.1f; //Speed of manual camera rotation
	const int kCameraLimit = 600; //Rotation limit on the X axis (prevents camera from looking too far up or down)
	const float kYLimit = 2.0f;

	const float kShakeDuration = 0.2f; //Stop shaking after this much time
	const float kShakeHeight = 0.5f; //Range of the shaking
	const float kShakeFPHeight = 0.1f; //Range of the shaking in first person mode
	const float kShakeSpeed = 14.0f; //Speed of the shaking
	const float kShakeFPSpeed = 1.8f; //Speed of the shaking in first person mode

	//Camera
	ICamera* camera;
	IModel* dummy; //Dummy to attach camera to
	int modeIndex = 0; //Current mode selected

	//Frame time
	float fTime; //Frame time

	//Mouse capture
	bool mouseCaptureOn = true;
	int mouseMoveX; //Mouse X rotation/Camera Y rotation
	int mouseMoveY; //Mouse Y rotation/Camera X rotation

	//Shake
	float shakeTimer = 0.0f; //Keeps track of how long the camera has been shaking for
	float yGoal = 0.0f; //Local Y position the camera should be at

	//Functions
	Camera(I3DEngine* e, IMesh* dummyMesh, HoverCar player); //Constructor

	void Controls(I3DEngine* e, HoverCar *player); //Take key input to move the camera and change its modes
	void SetMode(int i); //Use the passed index to select one of the modes and set the camera position and rotation accordingly
	void Shake(); //Trigger camera shaking
	void Update(I3DEngine* e, float frameTime, HoverCar *player); //Update frame time and take input
};

struct BoundingBox
{
	float xStart;
	float xEnd;
	float zStart;
	float zEnd;

	BoundingBox(float xPos = 0.0f, float zPos = 0.0f, float halfWidth = 0.0f, float halfLength = 0.0f); //Constructor
	void Initialise(float xPos, float zPos, float halfWidth, float halfLength); //Separated to make an early definition possible (needed for checkpoints)

	ColAxis Collision(HoverCar *car); //Collision detection with a hover car, returns collision direction
};

struct BoundingSphere
{
	float x;
	float z;
	float r;

	BoundingSphere(float xPos, float zPos, float radius); //Constructor

	bool Collision(HoverCar *car); //Collision detection with a hover car
};

struct Object
{
	IModel* m;

	Object(IMesh* mesh, float x, float y, float z, float r);
};

struct Checkpoint
{
	const float kCrossScale = 0.25f; //Scale of the cross model
	const float kCrossHeight = 4.8f; //Height at which the cross is positioned
	const float kCrossTime = 3.0f; //Time the cross appears for

	IModel* m; //Checkpoint model
	BoundingBox check; //Bounding box for detecting checkpoint crossing
	BoundingBox checkWide; //Bounding box for detecting checkpoint crossing by the AI

	IModel* cross; //Cross model
	float timer = 0.0f; //Cross timer

	Checkpoint(IMesh* checkpointMesh, IMesh* crossMesh, float x, float y, float z, float r); //Constructor

	void ShowCross(); //Show a cross to signify that the checkpoint has been crossed
	void HideCross(); //Hide the cross underground

	void Update(float fTime); //Update cross timer and hide it when time runs out
};

struct UI
{
	//Media
	const string kSpriteUIfront = "ui_front.png";
	const string kSpriteUIboost[5] = { "ui_boost.png", "ui_boost2.png", "ui_boost3.png", "ui_boost4.png", "ui_boost5.png" };
	const string kSpriteUIback = "ui_back.png";
	const string kSpriteUIend = "end_backdrop.png";

	//Size and position
	const float kBackdropHeight = 67.0f;
	const string kUIFont = "Consolas";
	const int kUIStatusFontSize = 28;
	const int kUIFontSize = 18;
	const int kUIEndFontSize = 44;
	const int kUITextHeight = (int)(kWindowSize.z - kBackdropHeight / 2 + 3); //Y position of the UI text
	const int kUITextSpace = 10;//If there are two lines this is half the distance between them
	const float kUIBackX = 524;
	const int kStatusX = 80;
	const int kLapX = 829;
	const int kHealthX = 962;
	const int kSpeedX = 1121;
	const int kBoostX = 637;
	const int kBoostLen = 233;
	const float kBoostLevel[4] = { 1.5f, 1.0f, 0.6f, 0.3f }; //When boost time reaches these values the colour of the boost bar changes
	const float kBoostTextTime = 1.2f; //Intervals between the boost status text flashing 
	const float kBoostTextFlashTime = 0.15f;
	const float kEndSpriteY = 291.0f;
	const int kEndTextY = int(kWindowSize.z / 2 - 40);
	const int kEndText2Y = int(kWindowSize.z / 2 - 2);

	//Frame time
	float fTime;

	//Sprites and fonts
	ISprite* uiFront;
	ISprite* uiBoost[5]; //Sprites for each colour of the boost bar
	ISprite* uiBack;
	ISprite* uiEnd;
	IFont* uiStatusFont;
	IFont* uiFont;
	IFont* uiEndFont;

	//Text holders
	stringstream status;
	stringstream lap;
	stringstream health;
	stringstream speed;
	stringstream time;
	stringstream boost;
	stringstream pos;
	stringstream endStatus;
	stringstream endStatus2;

	//Status
	int hpColour; //Changes to magenta when health is low
	bool end = 0; //True if player dies or finishes race

	//Time
	float countdown = -1.0f;
	float boostTimer = -1.0f;

	//Functions
	UI(I3DEngine* e); //Constructor
	void Reset(); //Reset the UI to its state from before the race started

	void ShowEndStatus(); //Make the end backdrop and text visible, triggered on death and race completion

	void UpdateWinner(string name, Time t); //When the first car completes a race the end text is updated with its name and time
	void UpdateStatus(int nCheck, int cLap, int lastCheck); //Updates to the status message, triggered when crossing checpoints
	void UpdateHP(int hp); //After a damage check the hp status is updated to show player's current hp
	void UpdateGeneral(float s, Time t, int playerPos, int carNumber); //Update to speed, time elapsed and race position text
	void UpdateBoost(float bTime); //Boost bar update, takes player's boost time

	bool Countdown(); //Countdown at the start of race, returns true when it finishes
	void GameOver(); //Updates text and shows end status when the player dies

	void Update(float frameTime, float boostTime); //Display all UI text and update boost bar
};

enum BombState { active, inactive, exploding };

struct Bomb
{
	BombState state = active; //Current state of the bomb

	const string kDefSkin = "Bomb.png";
	const string kExplosionSkin = "Bomb2.png";

	const float kBombScale = 1.1f; //Scale of the bomb model
	const float kBombXRot = 90.0f; //Rotation of the model on the X axis
	const float kBombYPos = 0.3f; //Position on the Y axis

	const float kBombRadius = 1.6f; //Radius of the trigger area
	const float kExplosionRadius = 10.0f; //Radius of the explosion area

	const float kCooldown = 40.0f; //Time before a bomb respawns
	const float kExplosionTime = 0.5f; //Duration of the explosion

	IModel* bomb; //Bomb model
	vector <BoundingSphere> colSphere; //Collidiong with this area triggers the bomb
	vector <BoundingSphere> explosionRange; //Range of the explosion
	vector <ExplosionEmitter> explosionParticles; //Explosion

	float cd = 0.0f; //Cooldown
	float eTime = 0.0f; //Explosion duration

	//Functions
	Bomb(IMesh* bombMesh, IMesh* particleMesh, float x, float z, float r); //Constructor

	void Trigger(); //Trigger the explosion
	void Explosion(); //Change state to inactive if explosion ended
	void Deactivate(); //Hide the bomb and set a cooldown
	void Reset(); //Activate the bomb and put it in sight

	void Update(float fTime, ICamera* camera); //Update timers and explosion particles
};

struct GridSquare //A piece of grid that holds obstacles
{
	const float kSpeedPointRange = 3.0f; //Range at which speed points are reacted to

	//Collision areas in the grid square
	vector <BoundingBox> boxObstacle;
	vector <BoundingSphere> sphereObstacle;

	//Points on the track where the AI speed changes
	vector <BoundingSphere> slowPoint;
	vector <BoundingSphere> fastPoint;

	//Fire zones
	vector <BoundingSphere> fire;
};

Vector2D GetCoord(float x, float z); //Used to obtain coordinates based on a position

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder(kMediaFolder);

	//Set seed for the random number generator
	srand(int(time(NULL)));

	/**** Set up your scene here ****/

	//Object meshes
	IMesh* hillsMesh = myEngine->LoadMesh(kMeshHills);
	IMesh* isleMesh = myEngine->LoadMesh(kMeshIsle);
	IMesh* isle2Mesh = myEngine->LoadMesh(kMeshIsle2);
	IMesh* wallMesh = myEngine->LoadMesh(kMeshWall);
	IMesh* checkpointMesh = myEngine->LoadMesh(kMeshCheckpoint);
	IMesh* crossMesh = myEngine->LoadMesh(kMeshCross);
	IMesh* walkwayMesh = myEngine->LoadMesh(kMeshWalkway);
	IMesh* tribuneMesh = myEngine->LoadMesh(kMeshTribune);
	IMesh* skyscraperMesh = myEngine->LoadMesh(kMeshSkyscraper);
	IMesh* skyscraper2Mesh = myEngine->LoadMesh(kMeshSkyscraper2);
	IMesh* buildingMesh = myEngine->LoadMesh(kMeshBuilding);
	IMesh* bushMesh = myEngine->LoadMesh(kMeshBush);
	IMesh* tank1Mesh = myEngine->LoadMesh(kMeshTank1);
	IMesh* tank2Mesh = myEngine->LoadMesh(kMeshTank2);
	IMesh* bombMesh = myEngine->LoadMesh(kMeshBomb);

	//Object arrays
	IModel* hills;
	vector <Object> isle;
	vector <Object> wall;
	vector <Object> walkway;
	vector <Object> building;
	vector <Object> bush;
	vector <Object> tank;
	vector <Checkpoint> checkpoint;
	vector <Bomb> bomb;

	vector<vector <Vector2D>> path; //Waypoints for the AI
	vector <Vector2D> emptyVector;
	for (int i = 0; i < kLaneNumber; i++) path.push_back(emptyVector); //Put empty vectors in to fill later

	vector <Vector2D> startPos; //Positions that cars start at

	GridSquare grid[kGridSquares][kGridSquares]; //Parts of the terrain

	//Particles
	IMesh* particleMesh = myEngine->LoadMesh("quad.x");
	vector<FireEmitter> fire;

	/*****Build level****/
	string type;
	float x;
	float z;
	float r;

	ifstream lFile;
	lFile.open(kLevelFile);

	while (!lFile.eof())
	{
		//Get "words" from file and put them in temporary variables
		lFile >> type;
		lFile >> x;
		lFile >> z;
		lFile >> r;

		Vector2D gs = GetCoord(x, z); //Grid square coordinates for the current object

		//Put the obtained object in an array that matches its type
		if (type == "Isle")
		{
			isle.push_back(Object(isleMesh, x, 0, z, r));
			if (r == 0 || r == 180) grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kIsleWid, kIsleLen));
			else grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kIsleLen, kIsleWid));
		}
		else if (type == "Isle2")
		{
			isle.push_back(Object(isle2Mesh, x, 0, z, r));
			if (r == 0 || r == 180) grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kIsleWid, kIsleLen));
			else grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kIsleLen, kIsleWid));
		}
		else if (type == "Wall")
		{
			wall.push_back(Object(wallMesh, x, 0, z, r));
			if (r == 0 || r == 180) grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kWallWid, kWallLen));
			else grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kWallLen, kWallWid));
		}
		else if (type == "Checkpoint")
		{
			checkpoint.push_back(Checkpoint(checkpointMesh, crossMesh, x, 0, z, r));
			if (r == 0 || r == 180)
			{
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x - kCheckpointLen + kCheckpointRad, z, kCheckpointRad));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x + kCheckpointLen - kCheckpointRad, z, kCheckpointRad));

				if (checkpoint.size() == 1) for (int i = 0; i < kMaxCars; i++) //If it's the first checkpoint add start positions
				{
					if (r == 0) startPos.push_back({ x + kStartPositions[i], z + kStartPosDistance });
					else startPos.push_back({ x + kStartPositions[i], z - kStartPosDistance });
				}
			}
			else
			{
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x, z - kCheckpointLen + kCheckpointRad, kCheckpointRad));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x, z + kCheckpointLen - kCheckpointRad, kCheckpointRad));

				if (checkpoint.size() == 1) for (int i = 0; i < kMaxCars; i++) //If it's the first checkpoint add start positions
				{
					if (r == 90) startPos.push_back({ x + kStartPosDistance, z + kStartPositions[i] });
					else startPos.push_back({ x - kStartPosDistance, z + kStartPositions[i] });
				}
			}
		}
		else if (type == "Hills")
		{
			hills = hillsMesh->CreateModel(x, kHillY, z);
			hills->RotateY(r);
			hills->Scale(kHillScale);
		}
		else if (type == "Walkway")
		{
			walkway.push_back(Object(walkwayMesh, x, 0, z, r));
			walkway.back().m->Scale(kWalkwayScale);
		}
		else if (type == "Tank1")
		{
			tank.push_back(Object(tank1Mesh, x, 0, z, r));
			tank.back().m->Scale(kTankScale);
			grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x, z, kTankRad));
		}
		else if (type == "Tank2")
		{
			tank.push_back(Object(tank2Mesh, x, kTank2Y, z, r));
			tank.back().m->Scale(kTankScale);
			tank.back().m->RotateLocalX(kTank2Rot);
			grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x, z, kTankRad));
			fire.push_back(FireEmitter(particleMesh, { x, kTankFireHeight, z }));
			grid[int(gs.x)][int(gs.z)].fire.push_back(BoundingSphere(x, z, kTankFireRad + 0.1f));
		}
		else if (type == "Skyscraper")
		{
			building.push_back(Object(skyscraperMesh, x, 0, z, r));
			building.back().m->Scale(kSkyscraperScale);

			float adjustment; //Model has to be moved a little because its center is not in the mesh's origin
			if (r == 0 || r == 90) adjustment = kSkyscraperAdjustment;
			else adjustment = -kSkyscraperAdjustment;

			if (r == 0 || r == 180)
			{
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z + adjustment, kSkyscraperLength1, kSkyscraperWidth1));
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z + adjustment, kSkyscraperLength2, kSkyscraperWidth2));
			}
			else
			{
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x + adjustment, z, kSkyscraperWidth1, kSkyscraperLength1));
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x + adjustment, z, kSkyscraperWidth2, kSkyscraperLength2));
			}
		}
		else if (type == "Skyscraper2")
		{
			building.push_back(Object(skyscraper2Mesh, x, 0, z, r));
			building.back().m->Scale(kSkyscraper2Scale);

			if (r == 0)
			{
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kSkyscraper2Length, kSkyscraper2Width));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x + kSkyscraper2Length - kSkyscraper2Radius, z + kSkyscraper2Radius, kSkyscraper2Radius));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x + kSkyscraper2Length - kSkyscraper2Radius, z - kSkyscraper2Radius, kSkyscraper2Radius));
			}
			else if (r == 180)
			{
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kSkyscraper2Length, kSkyscraper2Width));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x - kSkyscraper2Length + kSkyscraper2Radius, z + kSkyscraper2Radius, kSkyscraper2Radius));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x - kSkyscraper2Length + kSkyscraper2Radius, z - kSkyscraper2Radius, kSkyscraper2Radius));
			}
			else if (r == 90)
			{
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kSkyscraper2Width, kSkyscraper2Length));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x + kSkyscraper2Radius, z - kSkyscraper2Length + kSkyscraper2Radius, kSkyscraper2Radius));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x - kSkyscraper2Radius, z - kSkyscraper2Length + kSkyscraper2Radius, kSkyscraper2Radius));
			}
			else if (r == 270)
			{
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kSkyscraper2Width, kSkyscraper2Length));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x + kSkyscraper2Radius, z + kSkyscraper2Length - kSkyscraper2Radius, kSkyscraper2Radius));
				grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x - kSkyscraper2Radius, z + kSkyscraper2Length - kSkyscraper2Radius, kSkyscraper2Radius));
			}
		}
		else if (type == "Building")
		{
			building.push_back(Object(buildingMesh, x, 0, z, r));
			building.back().m->Scale(kBuildingScale);

			grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x, z, kBuildingWidth, kBuildingWidth)); //Big box
			for (int i = -1; i < 2; i += 2) for (int j = -1; j < 2; j += 2)
				grid[int(gs.x)][int(gs.z)].boxObstacle.push_back(BoundingBox(x + i * kBuildingWidth - i, z + j * kBuildingWidth - j, kBuildingWidh2, kBuildingWidh2)); //Small edge boxes
		}
		else if (type == "Tribune")
		{
			building.push_back(Object(tribuneMesh, x, 0, z, r));
			building.back().m->Scale(kTribuneScale);
			grid[int(gs.x)][int(gs.z)].sphereObstacle.push_back(BoundingSphere(x, z, kTribuneRad));
		}
		else if (type == "Smallestbush")
		{
			bush.push_back(Object(bushMesh, x, 0, z, r));
			bush.back().m->Scale(kBushScale[0]);
		}
		else if (type == "Smallbush")
		{
			bush.push_back(Object(bushMesh, x, 0, z, r));
			bush.back().m->Scale(kBushScale[1]);
		}
		else if (type == "Bush")
		{
			bush.push_back(Object(bushMesh, x, 0, z, r));
			bush.back().m->Scale(kBushScale[2]);
		}
		else if (type == "Bigbush")
		{
			bush.push_back(Object(bushMesh, x, 0, z, r));
			bush.back().m->Scale(kBushScale[3]);
		}
		else if (type == "Waypoint")
		{
			path[0].push_back({ x, z });
		}
		else if (type == "Waypoint2")
		{
			path[1].push_back({ x, z });
		}
		else if (type == "Slow")
		{
			grid[int(gs.x)][int(gs.z)].slowPoint.push_back(BoundingSphere(x, z, grid[int(gs.x)][int(gs.z)].kSpeedPointRange));
		}
		else if (type == "Fast")
		{
			grid[int(gs.x)][int(gs.z)].fastPoint.push_back(BoundingSphere(x, z, grid[int(gs.x)][int(gs.z)].kSpeedPointRange));
		}
		else if (type == "Bomb")
		{
			bomb.push_back(Bomb(bombMesh, particleMesh, x, z, r));
		}
	}
	lFile.close();

	//Add world edges as box obstacles
	for (int i = 1; i < kGridSquares - 1; i++) grid[1][i].boxObstacle.push_back(BoundingBox(-kWorldLen, 0, 0, kWorldLen));
	for (int i = 1; i < kGridSquares - 1; i++) grid[kGridSquares - 2][i].boxObstacle.push_back(BoundingBox(kWorldLen, 0, 0, kWorldLen));
	for (int i = 1; i < kGridSquares - 1; i++) grid[i][1].boxObstacle.push_back(BoundingBox(0, -kWorldLen, kWorldLen, 0));
	for (int i = 1; i < kGridSquares - 1; i++) grid[i][kGridSquares - 2].boxObstacle.push_back(BoundingBox(0, kWorldLen, kWorldLen, 0));

	/******Basic setup*****/
	IMesh* skyMesh = myEngine->LoadMesh(kMeshSky);
	IModel* sky = skyMesh->CreateModel(kPosSky.x, kPosSky.y, kPosSky.z);

	IMesh* groundMesh = myEngine->LoadMesh(kMeshGround);
	IModel* ground = groundMesh->CreateModel(0, 0, 0);

	//Cars
	IMesh* dummyMesh = myEngine->LoadMesh(kMeshDummy);
	IMesh* carMesh = myEngine->LoadMesh(kMeshCar);

	vector <HoverCar> cars;
	int numOfCars = kMaxCars;

	random_shuffle(startPos.begin(), (startPos.begin() + startPos.size() - 1)); //Shuffle the vector of starting positions to make the cars start at random spots

	for (int i = 0; i < numOfCars; i++) //Create cars at random positions
	{
		Vector2D sPos = startPos[i];
		stringstream n;
		n << "CAR" << (i + 1);
		if (i == 0) cars.push_back(HoverCar(dummyMesh, carMesh, particleMesh, path, sPos.x, sPos.z, "YOU", i, 0));
		else cars.push_back(HoverCar(dummyMesh, carMesh, particleMesh, path, sPos.x, sPos.z, n.str(), i, 1));
	}

	Camera camera(myEngine, dummyMesh, cars[0]);

	UI ui(myEngine);

	//States
	enum GameState { start, race, over };
	GameState gameState = start; //Overall state of the game, changes to over if player car dies or finishes race
	GameState raceState = start; //State of the race, changes to over if any car finishes the race

	//Frame speed tracker
	float frameTime;
	myEngine->Timer();

	float updateSpeed = 0.0f; //Used to limit the frequency of UI speed updates

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())

	{
		// Draw the scene
		myEngine->DrawScene();
		frameTime = myEngine->Timer(); //Get number of frames needed to draw scene

		/**** Update your scene each frame here ****/

		//Particles
		if (fire.size() > 0) for (size_t i = 0; i < fire.size(); i++) fire[i].Update(frameTime, &camera.camera, 1); //Update each fire emitter's particles

		//Start
		if (gameState == start)
		{
			if (myEngine->KeyHit(kKeyStart) && ui.countdown == -1) ui.countdown = kMaxCount; //Start the countdown
			if (ui.countdown >= 0) if (ui.Countdown())
			{
				//After countdown passes start race
				gameState = race;
				raceState = race;
			}
		}
		//Race
		else if (gameState == race)
		{
			//Car input
			cars[0].Controls(myEngine); //Take input to move the player car

			//Car timer
			for (int i = 0; i < numOfCars; i++) cars[i].UpdateTime();

			//AI movement
			for (int i = 1; i < numOfCars; i++) cars[i].AIFollowPath();

			//Checkpoint checks
			for (int i = 0; i < numOfCars; i++)
			{
				//If it's AI then the checkpoint doesn't actually need to be crossed - a wider collision box is used for the ckeckpoint
				if ((i == 0 && checkpoint[cars[i].nextCheck].check.Collision(&cars[i]) != none) || (i > 0 && checkpoint[cars[i].nextCheck].checkWide.Collision(&cars[i]) != none))
				{
					if (i == 0) checkpoint[cars[0].nextCheck].ShowCross();

					cars[i].nextCheck++;

					if (cars[i].nextCheck >= checkpoint.size())
					{
						cars[i].nextCheck = 0;
						cars[i].lap++;

						if (cars[i].lap > kLaps) //If finished race
						{
							if (raceState == race)
							{
								ui.UpdateWinner(cars[i].name, GetTime(cars[i].raceTime)); //Set end message
								raceState = over; //The winner can't be overridden
							}

							if (i == 0) //End game if player
							{
								ui.ShowEndStatus(); //Start showing end message
								gameState = over;
							}
						}
					}
					if (i == 0) ui.UpdateStatus(cars[0].nextCheck, cars[0].lap, checkpoint.size()); //Update status to reflect position changes
				}
			}
		}
		//Over
		else if (gameState == over)
		{
			//Move cars
			for (int i = 0; i < numOfCars; i++) cars[i].AIFollowPath(); //All cars that are not dead are controlled by computer

			//Reset level
			if (myEngine->KeyHit(kKeyRestart))
			{
				//Change game state
				gameState = start;
				raceState = start;

				//Reset cars
				random_shuffle(startPos.begin(), (startPos.begin() + startPos.size() - 1)); //Shuffle the vector of starting positions to make the cars start at random spots
				for (int i = 0; i < numOfCars; i++)
				{
					Vector2D sPos = startPos[i];
					stringstream n;
					cars[i].Reset(sPos.x, sPos.z);
				}

				//Reset checkpoints
				for (size_t i = 0; i < checkpoint.size(); i++) checkpoint[i].HideCross();

				//Reset UI
				ui.Reset();
			}
		}

		//Compare race position
		for (int i = 0; i < numOfCars; i++) for (int j = 0; j < numOfCars; j++) //Check each pair of cars

			if (i != j) cars[i].ComparePosition(&cars[j], checkpoint[cars[i].nextCheck].m); //Compare if it's a different car


		//Update
		updateSpeed += frameTime; //Timer used to limit speed updates
		if (updateSpeed > kUpPerSec)
		{
			ui.UpdateGeneral(sqrt(cars[0].momentum.Length()) * kScale * kMpsToKmph, GetTime(cars[0].raceTime), cars[0].racePos, numOfCars); //Show current speed
			updateSpeed = 0.0f;
		}
		ui.Update(frameTime, cars[0].boostTimer); //Show updated UI text

		for (int i = 0; i < numOfCars; i++) cars[i].Update(frameTime, &camera.camera); //Move cars according to their momentums
		camera.Update(myEngine, frameTime, &cars[0]); //Move camera

		for (size_t i = 0; i < checkpoint.size(); i++) checkpoint[i].Update(frameTime); //Update checkpoint (make cross disappear)

		//Collision detection
		for (int i = 0; i < numOfCars; i++)
		{
			float x = cars[i].dummy->GetX();
			float z = cars[i].dummy->GetZ();
			Vector2D gs = GetCoord(x, z); //Current grid square
			cars[i].currentSquare = gs;

			bool hit = 0; //True if there's a collision

			//Check the current and nearby squares for collisions
			for (int k = -1; k <= 1; k++) if (int(gs.x) + k >= 0 && int(gs.x) + k <= kGridSquares - 1) for (int l = -1; l <= 1; l++) if (int(gs.z) + l >= 0 && int(gs.z) + l <= kGridSquares - 1)
			{
				//Fire collision
				if (grid[int(gs.x) + k][int(gs.z) + l].fire.size() > 0) //If there are fires in the square
					for (size_t j = 0; j < grid[int(gs.x) + k][int(gs.z) + l].fire.size(); j++) //Go through each
					{
						if (grid[int(gs.x) + k][int(gs.z) + l].fire[j].Collision(&cars[i])) //If collision occurred
						{
							cars[i].burnTimer = cars[i].kBurnTime; //Update burn time
							break;
						}
					}

				//Sphere collision
				if (grid[int(gs.x) + k][int(gs.z) + l].sphereObstacle.size() > 0) //If there are sphere obstacles in the grid square
				{
					for (size_t j = 0; j < grid[int(gs.x) + k][int(gs.z) + l].sphereObstacle.size(); j++) //Go through each
					{
						if (grid[int(gs.x) + k][int(gs.z) + l].sphereObstacle[j].Collision(&cars[i])) //If collision occurred
						{
							cars[i].SphereCollision(j); //Change momentum and apply damage

							hit = 1;
							break; //Break to avoid getting stuck between two objects
						}
					}
				}

				//Box collision
				if (!hit && grid[int(gs.x) + k][int(gs.z) + l].boxObstacle.size() > 0) //If no collision was detected before and there are box obstacles in the grid square
				{
					for (size_t j = 0; j < grid[int(gs.x) + k][int(gs.z) + l].boxObstacle.size(); j++) //Go through each
					{
						ColAxis a = grid[int(gs.x) + k][int(gs.z) + l].boxObstacle[j].Collision(&cars[i]); //Check if collision happened and at what direction
						if (a != none)  //If collision occurred
						{
							cars[i].BoxCollision(j, a); //Change momentum and apply damage

							hit = 1;
							break; //Break to avoid getting stuck between two objects
						}
					}
				}

				//Car collision
				if (!hit) for (int m = 0; m < numOfCars; m++)  //If no collision was detected before and there are other cars nearby
					if (m != i && cars[m].currentSquare.x == gs.x + k && cars[m].currentSquare.z == gs.z + l && cars[m].colIndexCar != i) //Check for collision with cars on this square
					{
						if (cars[i].CarCollision(&cars[m], m)) break; //If collided with another car stop checking against other cars (in case two cars are close
					}

				//AI speed change
				if ((i != 0 || gameState == over) && grid[int(gs.x) + k][int(gs.z) + l].slowPoint.size() > 0) //If car is an AI an it came within the range of a slow point
					for (size_t j = 0; j < grid[int(gs.x) + k][int(gs.z) + l].slowPoint.size(); j++) //For each slow point in the grid square
						if (grid[int(gs.x) + k][int(gs.z) + l].slowPoint[j].Collision(&cars[i])) //If car is within range
							cars[i].AINewSpeed(slow); //Randomly change the thrust multiplier to something within the range of low speeds

				if ((i != 0 || gameState == over) && grid[int(gs.x) + k][int(gs.z) + l].fastPoint.size() > 0) //If car is an AI an it came within the range of a fast point
					for (size_t j = 0; j < grid[int(gs.x) + k][int(gs.z) + l].fastPoint.size(); j++) //For each fast point in the grid square
						if (grid[int(gs.x) + k][int(gs.z) + l].fastPoint[j].Collision(&cars[i])) //If car is within range
							cars[i].AINewSpeed(fast); //Randomly change the thrust multiplier to something within he range of high speeds
			}

			//Bomb and explosion collision
			if (bomb.size() > 0) for (size_t j = 0; j < bomb.size(); j++)
			{
				//Trigger explosion if car comes close to the bomb
				if (bomb[j].state == active && bomb[j].colSphere[0].Collision(&cars[i]))
				{
					bomb[j].Trigger();
				}
				if (bomb[j].state == exploding && bomb[j].explosionRange[0].Collision(&cars[i])) //Any car in the range of explosion gets damaged
				{
					cars[i].Explosion(&bomb[j].bomb);
					if (i == 0) camera.Shake();
				}
				bomb[j].Update(frameTime, camera.camera);
			}

		}

		//Update UI with current HP, end game if it went below 0
		if (cars[0].hp > 0)
		{
			ui.UpdateHP(cars[0].hp);
		}
		else
		{
			ui.UpdateHP(0);
			gameState = over;
			ui.GameOver();
		}

		//Quit
		if (myEngine->KeyHit(kKeyQuit))
		{
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

//Vector2D
float Vector2D::Length() //Returns length of a vector, but sqrt has to be applied separately as it's not needed in most cases
{
	return x * x + z * z;
}

Vector2D Vector2D::Normal() //Normalise vector
{
	return { x / sqrt(Length()), z / sqrt(Length()) };
}

Vector2D FacingVector(IModel* m) //Calculate the facing vector or a model
{
	float matrix[4][4];
	m->GetMatrix(&matrix[0][0]); //Store matrix in array
	return { matrix[2][0], matrix[2][2] }; //Z vector's X and Z values define the facing vector
}

Vector2D Vector2D::operator + (Vector2D v2) //Add two vectors together
{
	return { x + v2.x, z + v2.z };
}

Vector2D Vector2D::operator - (Vector2D v2) //Substract one vector from another
{
	return { x - v2.x, z - v2.z };
}

Vector2D Vector2D::operator - () //Reverse vector
{
	return { -x, -z };
}

Vector2D Vector2D::operator * (float scale) //Scalar multiplication
{
	return { x * scale, z * scale };
}

Vector2D Vector2D::operator * (Vector2D v2) //Multiply one vector by another
{
	return { x * v2.x, z * v2.z };
}

bool Vector2D::operator < (Vector2D v2) //Compare length of two vectors
{
	return Length() < v2.Length();
}

bool Vector2D::operator < (float f) //Compare length of a vector with a floating number
{
	return Length() < f;
}

bool Vector2D::operator > (Vector2D v2) //Compare length of two vectors
{
	return Length() > v2.Length();
}

bool Vector2D::operator > (float f) //Compare length of a vector with a floating number
{
	return Length() > f;
}

bool Vector2D::operator == (Vector2D v2) //Check if two vectors are equal
{
	return x == v2.x && z == v2.z;
}

//Hover Cars
HoverCar::HoverCar(IMesh* dummyMesh, IMesh* carMesh, IMesh* particleMesh, vector<vector<Vector2D>> &paths, float startX, float startZ, string carName, int carNo, bool ai) //Constructor
{
	//Setup
	dummy = dummyMesh->CreateModel();

	car = carMesh->CreateModel();
	car->Scale(kCarScale);
	car->AttachToParent(dummy);

	float y = kCarHoverHeight - kCarHoverRange + (rand() % 100) * 0.01f; //Get a random y position so that the cars move differently
	if (rand() % 2 == 1) bobbleDir = down; //50% chance for the car to start off by bobbling down instead of up

	dummy->SetPosition(startX, y, startZ);

	//Particle
	fire.push_back(FireEmitter(particleMesh, { startX, kCarHoverHeight + kBurnHeight, startZ }, kBurnRadius, kBurnVelRatio));
	smoke.push_back(SmokeEmitter(particleMesh, { startX, kCarHoverHeight + kSmokeHeight, startZ }, kSmokeRadius));
	exhaust.push_back(ExhaustEmitter(particleMesh, { startX, kCarHoverHeight + kExhaustHeight, startZ }, kExhaustRadius));

	//Skin
	if (ai) car->SetSkin(kSkinAI);
	else car->SetSkin(kSkinPlayer);

	//AI
	isAI = ai;
	goal = dummyMesh->CreateModel(startX, kCarHoverHeight, startZ);
	path = paths;
	if (pow(dummy->GetX() - path[0][0].x, 2) + pow(dummy->GetZ() - path[0][0].z, 2) < (pow(dummy->GetX() - path[1][0].x, 2) + pow(dummy->GetZ() - path[1][0].z, 2))) lane = 0;
	else lane = 1;
	nextWaypoint = dummyMesh->CreateModel(path[lane][0].x, kCarHoverHeight + kCarHoverHeight, path[lane][0].z);

	//Other
	racePos = carNo + 1; //Set a race position that's different to the other cars' so that the comparison can work
	name = carName; //Set a name
}

void HoverCar::Reset(float startX, float startZ) //Reset the car's variables and move it to a given starting position
{
	//Race
	raceTime = 0;
	nextCheck = 0;
	lap = 1;

	//Position and rotation
	float y = kCarHoverHeight - kCarHoverRange + (rand() % 100) * 0.01f; //Get a random y position so that the cars move differently
	if (rand() % 2 == 1) bobbleDir = down; //50% chance for the car to start off by bobbling down instead of up
	dummy->SetPosition(startX, y, startZ);
	dummy->ResetOrientation();
	car->ResetOrientation();

	tilt = 0;
	lean = 0;

	//Speed
	momentum = kZeroVector;
	thrust = kZeroVector;
	drag = kZeroVector;
	thMult = 1.0f;
	boostMult = 1.0f;
	drMult = 1.0f;

	//Collision
	prevPos = kZeroVector;
	colIndexSphere = -1;
	colIndexBox = -1;
	colIndexCar = -1;

	//Boost
	boostTimer = kBoostTime;
	boostLock = 0;

	//Health and status
	hp = kMaxHP;
	burnTimer = 0.0f;
	burnDamageTimer = 0.0f;
	explosionTimer = 0.0f;

	//AI
	currentGoal = 0;
	goal->SetPosition(startX, kCarHoverHeight, startZ);
	if (pow(dummy->GetX() - path[0][0].x, 2) + pow(dummy->GetZ() - path[0][0].z, 2) < (pow(dummy->GetX() - path[1][0].x, 2) + pow(dummy->GetZ() - path[1][0].z, 2))) lane = 0;
	else lane = 1;
	nextWaypoint->SetPosition(path[lane][0].x, kCarHoverHeight, path[lane][0].z);
}

void HoverCar::AIFollowPath() //Update AI orientation and speed, move the goal dummy and change waypoints when needed
{
	if (hp > 0)
	{
		//Rotation
		dummy->LookAt(goal);
		goal->LookAt(nextWaypoint);

		Tilt(1);

		//Speed
		speedChangeCD -= fTime;
		if (newThrust > boostMult) boostMult += kThrustChange * fTime; //Adjust thrust multiplier to match new thrust
		else if (newThrust < boostMult) boostMult -= kThrustChange * fTime;

		thrust = fVector * kThrustFactor * thMult * boostMult * fTime; //Set thrust

		if (hp < 1) thrust = kZeroVector; //If dead don't move
		else if (isAI && hp < kLowHP * kMaxHP) thrust = thrust * kLowHPPenalty; //If below 30% hp slow down considerably (only applies to non-player cars)
		else if (hp < kLowHP * kMaxHP * 2) thrust = thrust * kMedHPPenalty; //If below 60% hp slow down a little

		thrust = thrust + thrust * kThrustBonus * (float)racePos; //Increase thrust if not first

		//Follow goal
		Vector2D v = path[lane][currentGoal];
		float dist = sqrt(pow(dummy->GetX() - goal->GetX(), 2) + pow(dummy->GetZ() - goal->GetZ(), 2)); //Distance between car and goal
		if (dist < 1.0f) dist = 1.0f;

		if (dist < kMaxGoalDist) goal->MoveLocalZ((kGoalSpeed / dist) * fTime); //If car is close enough keep moving the goal dummy forward
		if (pow(goal->GetX() - v.x, 2) + pow(goal->GetZ() - v.z, 2) <= kMaxGoalDist) AINextWaypoint(); //If goal dummy gets close to current waypoint switch to the next one
	}
}

void HoverCar::AINewSpeed(Speed speed) //Switch to a random speed in a slow or fast range
{
	if (speedChangeCD <= 0.0f) //If car hasn't changed passed a speed point recently
	{
		speedChangeCD = kSpeedChangeCD; //Reset cooldown

		bool change = rand() % 2; //50% chance of changing speed
		if (change)
		{
			if (speed = slow) newThrust = kMidThrust - float(rand() % (int(100 * (kMidThrust - kMinThrust))) / 100.0f); //New speed between min and mid
			else if (speed = fast) newThrust = kMidThrust + float(rand() % (int(100 * (kMinThrust - kMidThrust))) / 100.0f); //New speed between mid and max
		}
	}
}

void HoverCar::AINextWaypoint() //Switch to next waypoint on AI's path
{
	currentGoal++;
	if (currentGoal >= path[lane].size()) currentGoal = 0;

	Vector2D v = path[lane][currentGoal];
	nextWaypoint->SetPosition(v.x, kCarHoverHeight, v.z);
}

void HoverCar::AISwitchLane() //Switch to a different lane
{
	if (isAI)
	{
		//Change lane
		if (lane == 0) lane = 1;
		else lane = 0;

		//Make sure the right goal is being followed
		currentGoal--;
		AINextWaypoint();
	}
}

void HoverCar::ComparePosition(HoverCar *car2, IModel* &checkpoint) //Compare position in the race with another racer
{
	bool updatePos;
	if (racePos > (*car2).racePos)
	{
		//Compare laps
		if (lap > (*car2).lap) updatePos = 1;
		else if (lap < (*car2).lap) updatePos = 0;

		//Compare checkpoints
		else if ((nextCheck == 0 && (*car2).nextCheck != 0) || (nextCheck != 0 && nextCheck > (*car2).nextCheck)) updatePos = 1;
		else if ((nextCheck != 0 && (*car2).nextCheck == 0) || ((*car2).nextCheck != 0 && nextCheck < (*car2).nextCheck)) updatePos = 0;

		//Compare distance to next checpoint
		else
		{
			float dist = pow((checkpoint->GetX() - dummy->GetX()), 2) + pow((checkpoint->GetZ() - dummy->GetZ()), 2);
			float dist2 = pow((checkpoint->GetX() - (*car2).dummy->GetX()), 2) + pow((checkpoint->GetZ() - (*car2).dummy->GetZ()), 2);
			if (dist < dist2) updatePos = 1;
			else updatePos = 0;
		}

		//Update position of each car
		if (updatePos)
		{
			int temp = racePos;
			racePos = (*car2).racePos;
			(*car2).racePos = temp;
		}
	}
}

void HoverCar::UpdateTime() //Update race time
{
	raceTime += fTime;
}

void HoverCar::UpdateDamage() //Damage related updates
{
	colDamage = int(floor(pow(sqrt(momentum.Length()) * kDamageFactor, 2))); //The faster the car is going the more damage it takes from collisions
	if (explosionTimer > 0) explosionTimer -= fTime;
}

void HoverCar::UpdateParticles(ICamera* *camera) //Update fire, smoke and exhaust fire particles coming from the car
{
	//Fire
	if (burnTimer > 0.0f) Burn(camera); //If car is burning show fire and take damage
	else fire[0].Update(fTime, camera, 0); //If it's not burning then just update the particles already spawned

	//Smoke
	if (hp < kLowHP * kMaxHP)
	{
		smoke[0].UpdateOrigin(Vector3D{ car->GetX() + fVector.x * kSmokeZPos, car->GetY() + kSmokeHeight, car->GetZ() + fVector.z * kSmokeZPos });
		smoke[0].Update(fTime, camera, 1, -momentum); //Emit smoke if hp is low
	}
	else smoke[0].Update(fTime, camera, 0, momentum * kSmokeMomentumMult); //Let smoke die off

	//Exhaust
	if (momentum.Length() > kExhaustMinSpeed && boostMult > kExhaustMinBoost)
	{
		exhaust[0].UpdateOrigin(Vector3D{ car->GetX() + fVector.x * kExhaustZPos, car->GetY() + kExhaustHeight, car->GetZ() + fVector.z * kExhaustZPos });
		exhaust[0].Update(fTime, camera, 1, -momentum);
	}
	else exhaust[0].Update(fTime, camera, 0, -momentum);
}

void HoverCar::Controls(I3DEngine* e) //Take keyboard input and react accordingly
{
	//Movement
	if (e->KeyHeld(kKeyCarForward))
	{
		thrust = fVector * kThrustFactor * thMult * boostMult * fTime; //Update thrust
		Tilt(1); //Tilt the car forward
	}
	else if (e->KeyHeld(kKeyCarBackward))
	{
		thrust = fVector * (-kThrustFactor / 2) * thMult * boostMult * fTime; //Update thrust
		Tilt(-1); //Tilt the car back
	}
	else
	{
		thrust = kZeroVector; //Reset thrust
	}

	//Steering
	if (e->KeyHeld(kKeyCarLeft))
	{
		dummy->RotateLocalY(-kCarRotation * fTime);
		Lean(1);
	}
	else if (e->KeyHeld(kKeyCarRight))
	{
		dummy->RotateLocalY(kCarRotation * fTime);
		Lean(-1);
	}

	//Boost
	Boost(e);
}

void HoverCar::TakeDamage(int damage) //Subtract damage and disable thrust if hp goes too low
{
	hp -= damage;
	if (hp <= 0) thMult = 0.0f;
}

void HoverCar::ResetCollision() //Return to normal speed and enable collisions when the car slows down enough
{
	if (thMult == 0.01f && momentum < kSlow)
	{
		thMult = 1.0f; //If momentum becomes low after thrust got locked enable it again
		colIndexSphere = -1; //Reset collision index so the recent sphere can be collided with again
		colIndexBox = -1; //Reset collision index so the recent box can be collided with again
		colIndexCar = -1; //Reset collision index so the recent car can be collided with again
	}
}

void HoverCar::SphereCollision(int index) //Collision with a sphere shaped obstacle
{
	if (index != colIndexSphere) //If it's not the object that already got collided with (prevents getting stuck in objects)
	{
		//Reset position to before collision occured
		dummy->SetX(prevPos.x);
		dummy->SetZ(prevPos.z);

		//Temporarily lower thrust and ignore object just collided with
		momentum = momentum * -1.0f; //Reverse momentum for a bounce back effect
		thMult = 0.01f; //Lower thrust
		colIndexSphere = index; //Record index of the object collided with
		colIndexBox = -1; //Reset box index in case of a bounceback

		//Damage car
		TakeDamage(colDamage);
	}
}

void HoverCar::BoxCollision(int index, ColAxis a) //Collision with a box shaped obstacle
{
	if (index != colIndexBox) //If it's not the object that already got collided with (prevents getting stuck in objects)
	{
		//Reset position to before collision occured
		dummy->SetX(prevPos.x);
		dummy->SetZ(prevPos.z);

		//Momentum change
		if (a == colX) momentum.x = momentum.x * -1; //If collided on Z axis reverse momentum on the Z axis
		else if (a == colZ) momentum.z = momentum.z * -1; //If collided on X reverse on X
		else momentum = momentum * -1; //If collided on both reverse completely

		//Temporarily lower thrust and ignore object just collided with
		thMult = 0.01f; //Lower thrust
		colIndexBox = index; //Record index of the object collided with
		colIndexSphere = -1; //Reset sphere index in case of a bounceback

		//Damage car
		TakeDamage(colDamage);
	}
	else if (isAI)
	{
		momentum = momentum * 2.0f; //Prevents AI cars from getting stuck
	}
}

bool HoverCar::CarCollision(HoverCar *car2, int index) //Collision with another car
{
	Vector2D dist = { (dummy->GetX() - (*car2).dummy->GetX()), (dummy->GetZ() - (*car2).dummy->GetZ()) };
	if (pow(dist.x, 2) + pow(dist.z, 2) - pow(r, 2) * kCarColRadiusMult < 0) //If cars overlap
	{
		//Reset position to before collision occured
		dummy->SetX(prevPos.x);
		dummy->SetZ(prevPos.z);
		(*car2).dummy->SetX((*car2).prevPos.x);
		(*car2).dummy->SetZ((*car2).prevPos.z);

		//Change momentums of the collided cars to make them bounce off a little
		dist = dist.Normal() * kCarColImpact; //Increased for a stronger bounce
		float change = (dummy->GetX() * dist.x + dummy->GetZ() * dist.z) - ((*car2).dummy->GetX() * dist.x + (*car2).dummy->GetZ() * dist.z);

		momentum = { change * dist.x, change * dist.z };
		(*car2).momentum = -momentum;

		//Temporarily lower thrust and ignore the car collided with
		thMult = 0.01f;
		colIndexCar = index;
		colIndexSphere = -1;
		colIndexBox = -1;

		//Damage cars (Damage is lowered because cars are lighter than immobile objects)
		int damage = (colDamage + (*car2).colDamage) / 3;
		TakeDamage(damage);
		(*car2).TakeDamage(damage);

		//If either or both cars are burning, combine their burn times and share the result between the cars
		float newBurnTime = burnTimer + (*car2).burnTimer;
		burnTimer = newBurnTime;
		(*car2).burnTimer = newBurnTime;

		//Change lane of the car behind to stop bumping into the other one's back
		if (lane == (*car2).lane)
		{
			if (racePos > (*car2).racePos) AISwitchLane();
			else (*car2).AISwitchLane();
		}
		return true;
	}
	else return false;
}

void HoverCar::Burn(ICamera* *camera) //Emit fire particles and take damage
{
	//Fire particles
	fire[0].UpdateOrigin(Vector3D{ car->GetX(), car->GetY() + kBurnHeight, car->GetZ() });
	fire[0].Update(fTime, camera, 1, -momentum);

	//Timers
	if (momentum.Length() > kBurnExtinguishSpeed) burnTimer -= fTime;
	else burnTimer -= fTime * kBurnExtinguisMult;

	burnDamageTimer += fTime;

	//Damage
	if (burnDamageTimer > kBurnDamageInterval)
	{
		TakeDamage(kBurnDamage);
		burnDamageTimer = 0.0f;
	}

	//Burning stops
	if (burnTimer < 0.0f)
	{
		burnDamageTimer = 0.0f; //Next time the car starts burning the interval will start over
	}
}

void HoverCar::Explosion(IModel* *bomb) //Push the car away from bomb and take damage
{
	if (explosionTimer <= 0.0f)
	{
		Vector2D dist = { (dummy->GetX() - (*bomb)->GetX()), (dummy->GetZ() - (*bomb)->GetZ()) };

		//Make sure the pushback isn't too strong or too weak
		float len = dist.Length();
		if (len < kBombMinDist) len = kBombMinDist;
		else if (len > kBombMaxDist) len = kBombMaxDist;

		dist = dist.Normal() * (kBombImpact / len); //Being closer to the bomb makes pushback stronger
		momentum = momentum - dist;

		TakeDamage(int(kBombDamage / len)); //Take more damage when close to bomb

		explosionTimer = kExplosionCooldown;
	}
}

void HoverCar::Move() //Move the car according to its momentum
{
	if (hp <= 0) thrust = kZeroVector; //Disable acceleration if dead
	drag = momentum * kDragCoefficient * drMult * fTime; //Calculate drag
	momentum = momentum + thrust + drag; //New momentum
	prevPos = { dummy->GetX(), dummy->GetZ() }; //Save previous postion
	dummy->Move(momentum.x * fTime, 0.0f, momentum.z * fTime); //Move dummy according to new momentum
}

void HoverCar::Rotate() //Update car's orientation based on lean and tilt values
{
	lean -= lean * kTiltDrag * fTime;
	tilt -= tilt * kTiltDrag * fTime;

	car->ResetOrientation();
	car->RotateLocalX(tilt);
	car->RotateLocalZ(lean);
}

void HoverCar::Bobble() //Move the car up and down
{
	if (car->GetY() > kCarHoverHeight + kCarHoverRange) bobbleDir = down; //If highest height reached change direction to down
	else if (car->GetY() < kCarHoverHeight - kCarHoverRange) bobbleDir = up; //If lowest change to up
	car->MoveY(kCarHoverSpeed * bobbleDir * fTime); //Move up or down depending on direction
}

void HoverCar::Tilt(float dir) //Update the tilt value, takes a direction multiplier of 1 or -1
{
	float change = kCarTiltFactor * dir * fTime; //Change in car's local X angle

	if (tilt + change < kCarMaxTilt && tilt + change > -kCarMaxTilt / 2) tilt += change; //Apply change if it doesn't make tilt go out of bounds
	else if (tilt + change > kCarMaxTilt) tilt = kCarMaxTilt;
	else if (tilt + change < -kCarMaxTilt / 2) tilt = -kCarMaxTilt / 2;

}

void HoverCar::Lean(float dir) //Update the lean value, takes a direction multiplier of 1 or -1
{
	float change = kCarLeanFactor * dir * fTime; //Change in car's local Z angle

	if (lean + change < kCarMaxLean && lean + change > -kCarMaxLean) lean += change; //Apply change if it doesn't make lean go out of bounds
	else if (lean + change > kCarMaxLean) lean = kCarMaxLean;
	else if (lean + change < -kCarMaxLean) lean = -kCarMaxLean;
}

void HoverCar::Boost(I3DEngine* e) //Checks performed when player attempts to use boost, along with consecutive actions
{
	if (hp >= kLowHP * kMaxHP) //If hp is above 30%
	{
		if (e->KeyHeld(kKeyBoost) && !boostLock && thrust.Length() > kBoostMinThrust) //When boost key is pressed, boost isn't locked and car isn't still
		{
			if (boostTimer > 0)
			{
				boostMult = kBoostMult;
				boostTimer -= fTime;
			}
			if (boostTimer <= 0) //If boost key is pressed too long overheat and lock the boost for 5 seconds
			{
				boostMult = 1.0f;
				boostTimer = kOverheatPenalty;
				boostLock = 1;
				drMult = 2.0f; //Double drag
			}
		}
		else
		{
			if (boostTimer < kBoostTime) //When key isn't pressed and boost timer isn't max increase it
			{
				boostTimer += fTime;
				if (boostLock) if (boostTimer >= 0)
				{
					boostLock = 0; //Remove lock after 5 seconds pass
					drMult = 1.0f; //Drag goes back to normal
				}
			}
			else boostTimer = kBoostTime; //If went over max set to max
			if (boostTimer >= 0) boostMult = 1.0f; //If not boosting and not overheat boost goes back to normal
		}
	}
	else //Boost permanently disabled if HP is below 30%
	{
		if (boostTimer < 0 && boostTimer > kOverheatPenalty) //If player was in overheat make sure the drag goes back to normal first
		{
			boostTimer += fTime;
		}
		else
		{
			drMult = 1.0f;
			boostTimer = -10.0f;
		}
		boostMult = 1.0f; //In case it was increased
		boostLock = 1; //Keep locked
	}
}

void HoverCar::Update(float frameTime, ICamera* *camera) //Actions performed every frame
{
	fTime = frameTime; //Get time to be used in movement

	//Movement
	ResetCollision();
	fVector = FacingVector(dummy);
	Move(); //Move the car according to its momentum
	Rotate(); //Apply changes in rotation
	Bobble(); //Up and down movement

	//Damage
	UpdateDamage();

	//Particles
	UpdateParticles(camera);
}

//UI
UI::UI(I3DEngine* e) //Constructor
{
	//Sprites
	uiBack = e->CreateSprite(kSpriteUIback, kUIBackX, kWindowSize.z - kBackdropHeight, 0.2f);
	uiFront = e->CreateSprite(kSpriteUIfront, 0.0f, kWindowSize.z - kBackdropHeight);
	for (int i = 0; i < 5; i++) uiBoost[i] = e->CreateSprite(kSpriteUIboost[i], kUIBackX, kWindowSize.z - kBackdropHeight, 0.1f);
	uiEnd = e->CreateSprite(kSpriteUIend, 0.0f, -kEndSpriteY); //Make sure it's not visible

	//Fonts
	uiStatusFont = e->LoadFont(kUIFont, kUIStatusFontSize);
	uiFont = e->LoadFont(kUIFont, kUIFontSize);
	uiEndFont = e->LoadFont(kUIFont, kUIEndFontSize);

	//Text
	status << "Hit Space to Start";
	lap << "Lap 1/" << kLaps;

	health << kMaxHP << "/" << kMaxHP << "HP";
	hpColour = kCyan;

	endStatus2 << "Press F1 to play again.";
}

void UI::Reset() //Reset the UI to its state from before the race started
{
	//Sprite reset
	uiEnd->SetY(-kEndSpriteY); //Hide end text

	//Text reset
	status.str("");
	status << "Hit Space to Start";

	lap.str("");
	lap << "Lap 1/" << kLaps;

	health.str("");
	health << kMaxHP << "/" << kMaxHP << "HP";
	hpColour = kCyan;

	//Time
	countdown = -1.0f;
	boostTimer = -1.0f;

	//Other
	end = 0;
}

void UI::ShowEndStatus() //Make the end backdrop and text visible, triggered on death and race completion
{
	uiEnd->SetY(kEndSpriteY); //Make sprite visible
	end = 1;
}

void UI::UpdateWinner(string name, Time t) //When the first car completes a race the end text is updated with its name and time
{
	endStatus.str("");
	endStatus << "RACE COMPLETE! " << name << " WON WITH A TIME OF " << setfill('0') << setw(2) << t.m << ":" << setfill('0') << setw(2) << t.s << ":" << setfill('0') << setw(2) << t.ms;
}


void UI::UpdateStatus(int nCheck, int cLap, int lastCheck) //Updates to the status message, triggered when crossing checpoints
{
	status.str("");
	if (cLap > kLaps) status << "Race complete!";
	else
	{
		if (nCheck == 0) nCheck = lastCheck; //If next lap is first in array then the one just passed was last in it
		status << "Stage " << nCheck << " complete";

		lap.str("");
		lap << "Lap " << cLap << "/" << kLaps;
	}

}

void UI::UpdateHP(int hp) //After a damage check the hp status is updated to show player's current hp
{
	if (hp < 0) hp = 0;
	health.str("");
	health << hp << "/" << kMaxHP << "HP";
	if (hp < kMaxHP * kLowHP) hpColour = kMagenta; //If hp is low change colour of the text
}

void UI::UpdateGeneral(float kmphSpeed, Time raceTime, int playerPos, int carNumber) //Update to speed, time elapsed and race position text
{
	//Speed
	speed.str("");
	speed << round(kmphSpeed) << "km/h";

	//Time elapsed
	time.str("");
	time << setfill('0') << setw(2) << raceTime.m << ":" << setfill('0') << setw(2) << raceTime.s << ":" << setfill('0') << setw(2) << raceTime.ms;

	//Position in race
	pos.str("");
	pos << "Pos " << playerPos << "/" << carNumber;
}

void UI::UpdateBoost(float bTime) //Boost bar update, takes player's boost time
{
	if (bTime < 0) boostTimer -= fTime; //The timer makes the overhead/boost down text flash, updated when it should be shown and flashing

	boost.str(""); //Clear text

	if (bTime < 0 && boostTimer < 0) //Show text if timer is below 0
	{
		if (bTime == -10) boost << "BOOST DOWN";
		else boost << "OVERHEAT";

		if (boostTimer <= -kBoostTextTime) boostTimer = kBoostTextFlashTime; //Reset timer
	}

	float boostPerSec = kBoostLen / kBoostTime; //Length of the boost bar that's depleted each second of use
	int colourIndex; //Used to change colour based on boost time left

	//Depending on boost timer change colour of the boost bar
	if (bTime >= kBoostLevel[0]) colourIndex = 0;
	else if (bTime < kBoostLevel[3]) colourIndex = 4;
	else if (bTime < kBoostLevel[2]) colourIndex = 3;
	else if (bTime < kBoostLevel[1]) colourIndex = 2;
	else if (bTime < kBoostLevel[0]) colourIndex = 1;

	for (int i = 0; i < 5; i++)
	{
		if (i == colourIndex) uiBoost[i]->SetX(kUIBackX - kBoostTime * boostPerSec + bTime * boostPerSec); //If boost is being used (not at max time) the bar will gradually hide
		else uiBoost[i]->SetX(0.0f); //Hide if not current colour
	}
}

bool UI::Countdown() //Countdown at the start of race, returns true when it finishes
{
	countdown -= fTime; //Update countdown
	status.str(""); //Clear string
	if (countdown > 0)
	{
		status << ceil(countdown) << "...";
		return 0;
	}
	else
	{
		status << "Go!";
		return 1;
	}
}

void UI::GameOver() //Updates text and shows end status when the player dies
{
	status.str("");
	status << "Game Over";

	endStatus.str("");
	endStatus << "GAME OVER";
	ShowEndStatus();
}

void UI::Update(float frameTime, float boostTime) //Display all UI text and update boost bar
{
	fTime = frameTime;
	UpdateBoost(boostTime);

	//Print status
	uiStatusFont->Draw(status.str(), kStatusX, kUITextHeight, kCyan, kLeft, kVCentre);

	//Print current lap
	uiFont->Draw(lap.str(), kLapX, kUITextHeight - kUITextSpace, kCyan, kLeft, kVCentre);

	//Print current position in race
	uiFont->Draw(pos.str(), kLapX, kUITextHeight + kUITextSpace, kCyan, kLeft, kVCentre);

	//Print hp amount 
	uiFont->Draw(health.str(), kHealthX, kUITextHeight, hpColour, kLeft, kVCentre);

	//Print current speed
	uiFont->Draw(speed.str(), kSpeedX, kUITextHeight - kUITextSpace, kCyan, kLeft, kVCentre);

	//Print current time
	uiFont->Draw(time.str(), kSpeedX, kUITextHeight + kUITextSpace, kCyan, kLeft, kVCentre);

	//Print boost status
	uiStatusFont->Draw(boost.str(), kBoostX, kUITextHeight, kMagenta, kCentre, kVCentre);

	//At the end of the race, display winner and their time
	if (end)
	{
		uiEndFont->Draw(endStatus.str(), int(kWindowSize.x / 2), kEndTextY, kMagenta, kCentre, kVCentre);
		uiStatusFont->Draw(endStatus2.str(), int(kWindowSize.x / 2), kEndText2Y, kCyan, kCentre, kVCentre);
	}
}

//Camera
Camera::Camera(I3DEngine* e, IMesh* dummyMesh, HoverCar player) //Constructor
{
	camera = e->CreateCamera(kManual, 0.0f, 0.0f, 0.0f);
	dummy = dummyMesh->CreateModel();
	camera->AttachToParent(dummy);
	dummy->AttachToParent(player.dummy);
	SetMode(0);
}

void Camera::Controls(I3DEngine* e, HoverCar *player) //Take key input to move the camera and change its modes
{
	//Movement
	if (e->KeyHeld(kKeyCameraUp)) dummy->MoveLocalZ(kCameraSpeed * fTime);
	if (e->KeyHeld(kKeyCameraDown)) dummy->MoveLocalZ(-kCameraSpeed * fTime);
	if (e->KeyHeld(kKeyCameraLeft)) dummy->MoveLocalX(-kCameraSpeed * fTime);
	if (e->KeyHeld(kKeyCameraRight)) dummy->MoveLocalX(kCameraSpeed * fTime);

	if (dummy->GetY() < kYLimit) camera->SetY(kYLimit);

	//Rotation
	dummy->ResetOrientation();
	mouseMoveX += e->GetMouseMovementX();
	mouseMoveY += e->GetMouseMovementY();
	dummy->RotateLocalY(mouseMoveX * kCameraRotation);
	dummy->RotateLocalX(mouseMoveY * kCameraRotation);

	if (mouseMoveY < -kCameraLimit) mouseMoveY = -kCameraLimit;
	else if (mouseMoveY > kCameraLimit) mouseMoveY = kCameraLimit;

	//Mouse capture
	if (e->KeyHit(kKeyCameraCapture)) mouseCaptureOn = !mouseCaptureOn;
	if (mouseCaptureOn) e->StartMouseCapture();
	else e->StopMouseCapture();

	//Chase mode
	if (e->KeyHit(kKeyCameraChase))
	{
		SetMode(0);
		dummy->DetachFromParent();
		dummy->AttachToParent((*player).dummy);
	}
	//First person mode
	else if (e->KeyHit(kKeyCameraFP))
	{
		SetMode(1);
		dummy->DetachFromParent();
		dummy->AttachToParent((*player).car);
	}
	//Free mode
	else if (e->KeyHit(kKeyCameraFree))
	{
		SetMode(2);
		dummy->DetachFromParent();
	}
}

void Camera::SetMode(int i) //Use the passed index to select one of the modes and set the camera position and rotation accordingly
{
	dummy->SetLocalPosition(kCameraModes[i].pos.x, kCameraModes[i].pos.y, kCameraModes[i].pos.z);
	mouseMoveX = kCameraModes[i].mouseX;
	mouseMoveY = kCameraModes[i].mouseY;
	modeIndex = i;
}

void Camera::Shake() //Trigger camera shaking
{
	shakeTimer = kShakeDuration;

	if (modeIndex == 1) yGoal = -kShakeFPHeight;
	else yGoal = -kShakeHeight;
}

void Camera::Update(I3DEngine* e, float frameTime, HoverCar *player) //Update frame time and take input
{
	fTime = frameTime;
	Controls(e, player);

	//Shake the camera
	shakeTimer -= fTime;
	if (shakeTimer > 0.0f)
	{
		if (modeIndex == 1) //If in first person mode shake less
		{
			if (camera->GetLocalY() > kShakeFPHeight) yGoal = -kShakeFPHeight;
			else if (camera->GetLocalY() < -kShakeFPHeight) yGoal = kShakeFPHeight;
		}
		else
		{
			if (camera->GetLocalY() > kShakeHeight) yGoal = -kShakeHeight;
			else if (camera->GetLocalY() < -kShakeHeight) yGoal = kShakeHeight;
		}
	}
	else yGoal = 0.0f;

	if (modeIndex == 1)
	{
		if (camera->GetLocalY() > yGoal) camera->MoveLocalY(-kShakeFPSpeed * fTime);
		else if (camera->GetLocalY() < yGoal) camera->MoveLocalY(kShakeFPSpeed * fTime);
	}
	else
	{
		if (camera->GetLocalY() > yGoal) camera->MoveLocalY(-kShakeSpeed * fTime);
		else if (camera->GetLocalY() < yGoal) camera->MoveLocalY(kShakeSpeed * fTime);
	}

	if (yGoal == 0.0f && pow(camera->GetLocalY() - yGoal, 2) < pow(kShakeSpeed * fTime, 2)) camera->SetLocalY(0);
}

//Collision detection
BoundingBox::BoundingBox(float xPos, float zPos, float halfWidth, float halfLength) //Constructor
{
	Initialise(xPos, zPos, halfWidth, halfLength);
}

void BoundingBox::Initialise(float xPos, float zPos, float halfWidth, float halfLength) //Separated to make an early definition possible (needed for checkpoints)
{
	xStart = xPos - halfWidth;
	xEnd = xPos + halfWidth;
	zStart = zPos - halfLength;
	zEnd = zPos + halfLength;
}

ColAxis BoundingBox::Collision(HoverCar *car) //Collision detection with a hover car, returns collision direction
{
	if (((*car).dummy->GetX() + (*car).r) > xStart && ((*car).dummy->GetX() - (*car).r) < xEnd && ((*car).dummy->GetZ() + (*car).r) > zStart && ((*car).dummy->GetZ() - (*car).r) < zEnd)
	{
		if ((*car).prevPos.x + (*car).r > xStart && (*car).prevPos.x - (*car).r < xEnd &&
			!((*car).prevPos.z + (*car).r > zStart && (*car).prevPos.z - (*car).r < zEnd)) return colZ; //If X overlaps col was on Z axis 
		else if ((*car).prevPos.z + (*car).r > zStart && (*car).prevPos.z - (*car).r < zEnd &&
			!((*car).prevPos.x + (*car).r > xStart && (*car).prevPos.x - (*car).r < xEnd)) return colX; //If Z then X axis
		else return both; //If collided at an angle then collision was on both axes
	}
	else return none;
}

BoundingSphere::BoundingSphere(float xPos, float zPos, float radius) //Constructor
{
	x = xPos;
	z = zPos;
	r = pow(radius, 2);
}

bool BoundingSphere::Collision(HoverCar *car) //Collision detection with a hover car
{
	return (pow(x - (*car).dummy->GetX(), 2) + pow(z - (*car).dummy->GetZ(), 2) - r - pow((*car).r, 2)) < 0; //Returns true if distance is smaller than 0
}

//Objects
Object::Object(IMesh* mesh, float x, float y, float z, float r) //Constructor
{
	m = mesh->CreateModel(x, y, z);
	m->RotateLocalY(r);
}

Checkpoint::Checkpoint(IMesh* checkpointMesh, IMesh* crossMesh, float x, float y, float z, float r) //Constructor
{
	m = checkpointMesh->CreateModel(x, y, z);
	m->RotateLocalY(r);

	cross = crossMesh->CreateModel(x, -kCrossHeight, z); //Cross spawned underground so that it's invisible
	cross->RotateLocalY(r);
	cross->Scale(kCrossScale);

	if (r != 0 && r != 180)
	{
		check.Initialise(m->GetX(), m->GetZ(), kCheckpointWidth, kCheckpointLen - kCheckpointRad * 2.0f); //Increase Z by length if the checkpoint is not at an angle
		checkWide.Initialise(m->GetX(), m->GetZ(), kCheckpointWidth, kCheckpointLen * 3.0f); //Wider version for the AI because they're stupid
	}
	else
	{
		check.Initialise(m->GetX(), m->GetZ(), kCheckpointLen - kCheckpointRad * 2.0f, kCheckpointWidth); //Increase X by length if the checkpoint is at an angle
		checkWide.Initialise(m->GetX(), m->GetZ(), kCheckpointLen * 3.0f, kCheckpointWidth); //Wider for the AI
	}
}

void Checkpoint::ShowCross() //Show a cross to signify that the checkpoint has been crossed
{
	cross->SetLocalY(kCrossHeight);
	timer = kCrossTime;
}

void Checkpoint::HideCross() //Hide the cross underground
{
	cross->SetLocalY(-kCrossHeight);
}

void Checkpoint::Update(float fTime) //Update cross timer and hide it when time runs out
{
	if (timer > 0.0f)
	{
		timer -= fTime;
		if (timer <= 0.0f) HideCross();
	}
}

Bomb::Bomb(IMesh* bombMesh, IMesh* particleMesh, float x, float z, float r) //Constructor
{
	bomb = bombMesh->CreateModel(x, kBombYPos, z);
	bomb->Scale(kBombScale);
	bomb->RotateX(kBombXRot);

	colSphere.push_back(BoundingSphere(x, z, kBombRadius));
	explosionRange.push_back(BoundingSphere(x, z, kExplosionRadius));
	explosionParticles.push_back(ExplosionEmitter(particleMesh, { x, kBombYPos, z }));
}

void Bomb::Trigger() //Trigger the explosion
{
	bomb->SetSkin(kExplosionSkin);
	state = exploding;
	eTime = kExplosionTime;
}

void Bomb::Explosion() //Change state to inactive if explosion ended
{
	if (eTime < 0.0f)
	{
		state = inactive;
		Deactivate();
	}
}

void Bomb::Deactivate() //Hide the bomb and set a cooldown
{
	bomb->SetY(-20.0f);
	cd = kCooldown;
}

void Bomb::Reset() //Activate the bomb and put it in sight
{
	bomb->SetY(kBombYPos);
	bomb->SetSkin(kDefSkin);
	state = active;
}

void Bomb::Update(float fTime, ICamera* camera) //Update timers and explosion particles
{
	if (state == exploding)
	{
		explosionParticles[0].Update(fTime, &camera, 1);
		eTime -= fTime;
		Explosion();
	}
	else explosionParticles[0].Update(fTime, &camera, 0);

	if (state == inactive)
	{
		cd -= fTime;
		if (cd < 0.0f) Reset();
	}
}


//Particles
void Particle::CreateParticle(IMesh* particleMesh, string skin, Vector3D particleOrigin, float originRadius, Vector3D startVelocity, float minLifetime, float maxLifetime, float minVelocity,
	ParticleType pType, float pAngle) //Create a new particle model and set its skin and variables
{
	particle = particleMesh->CreateModel();
	particle->SetSkin(skin);

	origin = particleOrigin;
	radius = originRadius;

	sv = startVelocity;
	angle = pAngle;

	maxLife = maxLifetime;
	minLife = minLifetime;
	minVel = pow(minVelocity, 2);

	Spawn();
}

float Particle::RandomAngle(float angle) //Generate a random angle within a specified range
{
	return (rand() % int(angle * 2000)) / 1000.0f - angle;
}

void Particle::Spawn(Vector2D momentum) //Reset a particle's position and velocity
{
	float distFromOrigin = float((rand() % int(radius * 100.0f)) / 100.0f); //Random distance from origin

	Vector3D pPos = { origin.x + distFromOrigin * float(cos(rand())), origin.y, origin.z + distFromOrigin * float(cos(rand())) };
	particle->SetPosition(pPos.x, pPos.y, pPos.z);

	if (type == fire) totalLife = float(rand() % (int((maxLife - minLife) * 1000)) / 1000.0f) * (radius / distFromOrigin); //Gives fire a triangle shape
	else totalLife = float(rand() % (int((maxLife - minLife) * 1000)) / 1000.0f);
	life = 0.0f;

	v = sv + sv * Vector3D{ momentum.x, 0.0f, momentum.z };

	if (angle != 0)
	{
		v.x += RandomAngle(angle);
		v.z += RandomAngle(angle);
	}
}

void Particle::Update(float fTime, Vector3D acceleration, Vector3D partOrigin, ICamera* *camera, bool isActive, Vector2D momentum, Vector3D newStartingVelocity) //Update orientation and velocity, 
																																								//move and respawn after death
{
	//Turn to camera
	particle->LookAt(*camera);

	//Update velocity
	v = v + acceleration * fTime;

	//Update origin
	origin = partOrigin;

	//Move the particle according to velocity
	if (particle->GetY() > 0.0f) particle->Move(v.x * fTime, v.y * fTime, v.z * fTime); //If not hidden (dead) keep moving

	//Update life and kill the particle
	life += fTime;
	if (life > totalLife || v.Length() <= minVel)
	{
		if (isActive)
		{
			Spawn(momentum);
			if (newStartingVelocity.Length() > 0.0f) sv = newStartingVelocity;
		}
		else particle->MoveY(-100.0f); //If particles aren't actively spawned, hide from sight
	}
}

//Emitters
ExplosionEmitter::ExplosionEmitter(IMesh* explosionMesh, Vector3D emitterOrigin, float explosionRadius, float velocityRatio) //Constructor
{
	radius = explosionRadius;
	velRatio = velocityRatio;

	eMesh = explosionMesh;
	origin = emitterOrigin;
}

void ExplosionEmitter::NewParticle() //Add a new particle to to the array of particles
{
	int skinIndex = rand() % explosionSkin.size();

	sVelocity = NewSV();

	particle[particleIndex].CreateParticle(eMesh, explosionSkin[skinIndex], { origin.x, kParticleHeight, origin.z }, radius, sVelocity, kMinLife, kMaxLife, kMinVel, explosion);
	particleIndex++;
}

Vector3D ExplosionEmitter::NewSV() //Generate a random starting vector so that the explosion particles can shoot in any direction
{
	//Create a randomised vector
	int range = 100;
	Vector3D v = { float(rand() % range) - float(range / 2), float(rand() % range), float(rand() % range) - float(range / 2) };

	//Normalise, multiply by speed and return
	return v.Normal() * kStartSpeed;
}

void ExplosionEmitter::Update(float fTime, ICamera* *camera, bool isActive, Vector2D momentum) //Spawn more particles and update the existing ones
{
	//Time
	timer += fTime;

	//Add new particles if limit hasn't been reached
	if (isActive && particleIndex < kMaxParticles - 1 && timer > kFrequency)
	{
		NewParticle();
		timer = 0.0f;
	}

	//Update existing particles
	if (particleIndex > 0) for (int i = 0; i < particleIndex; i++)
	{
		Vector3D velChange = particle[i].v * kAcceleration;
		particle[i].Update(fTime, velChange, { origin.x, kParticleHeight, origin.z }, camera, isActive, momentum, NewSV());
	}
}

void ExplosionEmitter::UpdateOrigin(Vector3D particleOrigin) //Change origin position if it had moved
{
	//Set origin to a new one
	origin = particleOrigin;
}

SmokeEmitter::SmokeEmitter(IMesh* fireMesh, Vector3D emitterOrigin, float smokeRadius, float velocityRatio) //Constructor
{
	radius = smokeRadius;
	velRatio = velocityRatio;

	sMesh = fireMesh;
	origin = emitterOrigin;
}

void SmokeEmitter::NewParticle() //Add a new particle to to the array of particles
{
	int skinIndex = rand() % smokeSkin.size();

	particle[particleIndex].CreateParticle(sMesh, smokeSkin[skinIndex], origin, radius, { sVelocity.x, sVelocity.y * velRatio, sVelocity.z }, kMinLife, kMaxLife, kMinVel, smoke, kAngle);
	particleIndex++;
}

void SmokeEmitter::Update(float fTime, ICamera* *camera, bool isActive, Vector2D momentum) //Spawn more particles and update the existing ones
{
	//Time
	timer += fTime;

	//Add new particles if limit hasn't been reached
	if (isActive && particleIndex < kMaxParticles - 1 && timer > kFrequency)
	{
		NewParticle();
		timer = 0.0f;
	}

	//Update existing particles
	Vector3D velChange = kAcceleration;

	if (particleIndex > 0) for (int i = 0; i < particleIndex; i++)
	{
		particle[i].Update(fTime, velChange, origin, camera, isActive, momentum);
	}
}

void SmokeEmitter::UpdateOrigin(Vector3D particleOrigin) //Change origin position if it had moved
{
	//Set origin to a new one
	origin = particleOrigin;
}

FireEmitter::FireEmitter(IMesh* fireMesh, Vector3D emitterOrigin, float fireRadius, float velocityRatio) //Constructor
{
	radius = fireRadius;
	velRatio = velocityRatio;

	fMesh = fireMesh;
	origin = emitterOrigin;
}

void FireEmitter::NewParticle() //Add a new particle to to the array of particles
{
	int skinIndex = rand() % fireSkin.size();

	particle[particleIndex].CreateParticle(fMesh, fireSkin[skinIndex], origin, radius, { sVelocity.x, sVelocity.y * velRatio, sVelocity.z }, kMinLife, kMaxLife, kMinVel, fire);
	particleIndex++;
}

void FireEmitter::NewParticle2() //Add a new particle to to the second array of particles
{
	int skinIndex = rand() % fire2Skin.size();

	particle2[particleIndex2].CreateParticle(fMesh, fire2Skin[skinIndex], origin, radius, { sVelocity2.x, sVelocity2.y * velRatio, sVelocity2.z }, kMinLife2, kMaxLife2, kMinVel2, fire);
	particleIndex2++;
}

void FireEmitter::Update(float fTime, ICamera* *camera, bool isActive, Vector2D momentum) //Spawn more particles and update the existing ones
{
	//Time
	timer += fTime;
	timer2 += fTime;

	//Add new particles if limit hasn't been reached
	if (isActive && particleIndex < kMaxParticles - 1 && timer > kFrequency)
	{
		NewParticle();
		timer = 0.0f;
	}
	if (isActive && particleIndex2 < kMaxParticles2 - 1 && timer2 > kFrequency)
	{
		NewParticle2();
		timer2 = 0.0f;
	}

	//Update existing particles
	Vector3D velChange = kGravity;

	if (particleIndex > 0) for (int i = 0; i < particleIndex; i++)
	{
		particle[i].Update(fTime, velChange, origin, camera, isActive, momentum);
	}
	if (particleIndex2 > 0) for (int i = 0; i < particleIndex2; i++)
	{
		particle2[i].Update(fTime, velChange, origin, camera, isActive, momentum);
	}
}

void FireEmitter::UpdateOrigin(Vector3D particleOrigin) //Change origin position if it had moved
{
	//Set origin to a new one
	origin = particleOrigin;
}

ExhaustEmitter::ExhaustEmitter(IMesh* exhaustMesh, Vector3D emitterOrigin, float exhaustRadius, float velocityRatio) //Constructor
{
	radius = exhaustRadius;
	velRatio = velocityRatio;

	eMesh = exhaustMesh;
	origin = emitterOrigin;
}

void ExhaustEmitter::NewParticle() //Add a new particle to to the array of particles
{
	int skinIndex = rand() % exhaustSkin.size();

	particle[particleIndex].CreateParticle(eMesh, exhaustSkin[skinIndex], origin, radius, { sVelocity.x, sVelocity.y * velRatio, sVelocity.z }, kMinLife, kMaxLife, kMinVel, exhaust, kAngle);
	particleIndex++;
}

void ExhaustEmitter::Update(float fTime, ICamera* *camera, bool isActive, Vector2D momentum) //Spawn more particles and update the existing ones
{
	//Time
	timer += fTime;
	timer2 += fTime;

	//Add new particles if limit hasn't been reached
	if (isActive && particleIndex < kMaxParticles - 1 && timer > kFrequency)
	{
		NewParticle();
		timer = 0.0f;
	}

	//Update existing particles
	Vector3D velChange = kGravity;

	if (particleIndex > 0) for (int i = 0; i < particleIndex; i++)
	{
		particle[i].Update(fTime, velChange, origin, camera, isActive, momentum);
	}
}

void ExhaustEmitter::UpdateOrigin(Vector3D particleOrigin) //Change origin position if it had moved
{
	//Set origin to a new one
	origin = particleOrigin;
}

//Conversion
Time GetTime(float seconds)//Given a number of seconds return time in hours, minutes and seconds
{
	int s = int(floor(seconds));
	int ms = int((seconds - s) * 100);
	if (s > 0)
	{
		int m = s / 60;
		return { m, s % 60, ms };
	}
	else return { 0, 0, ms };
}

Vector2D GetCoord(float x, float z) //Used to obtain coordinates based on a position
{
	return { floor((x + kTerrainSize / 2) / kGridSize), floor((z + kTerrainSize / 2) / kGridSize) };
}