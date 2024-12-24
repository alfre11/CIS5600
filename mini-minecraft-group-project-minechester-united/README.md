# Mini Minecraft Milestone 1
## Efficient Terrain Rendering and Chunking
> Author: Bo Sun

To make `Chunk` inherit from `Drawable`, the function `createVBOdata()` was implemented. This method loops through all the blocks in the chunk and tests each face to see if it is exposed. If it is, it uses the `addFace` lambda function to create VBO data for this face. As per the instructions, the VBO data is stored in the `INTERLEAVED` format, with each vertex having 3 pieces of information interleaved: position, normal, and color.

To make this `INTERLEAVED` format compatible with our shader program, the function `drawInterleaved()` was added to the `ShaderProgram` class. This was essentially a copy-paste of the normal `draw()` function with small modifications to the last two arguments (stride and offset) of `glVertexAttribPointer`.

Then, the `draw()` function of the `Terrain` class was changed to render chunks instead of individual blocks. All references to the previous `m_geomCube` and `m_chunkVBOsNeedUpdating` were deleted.

Finally, the `loadChunks()` function of the `Terrain` class was implemented, which takes in the player's position and uses the `instantiateChunkAt` to instantiate all currently unloaded chunks within a 32-block radius of the player. The `tick()` function within `MyGL` was updated to call `loadChunks()` on every frame.
## Player Movement
> Author: Alex Freeman

In order to create fluid movement, I altered the myGL keyPress and keyRelease events to update the Inputs struct that is passed into the tick function. I called the player tick function inside the myGL tick fuction with the calculated dT. The player tick then calls processInputs and computePhysics. I implemented processInputs to update the accleration vector and velocity vector based on inputs. Then in computePhysics I check for collisions by raycasting the velocity vector in each direction, and setting the velocity of the direction to zero if the player is going to collide in that direction. I had to check which of the x and z collisions were closer in order to allow movement along a wall (sliding). I created a constant negative y acceleration when not in flight mod to represent gravity, and set a positive y velocity when spacebar is pressed for jumping. When the mouse buttons are pressed, I check whether there is a solid block in range based on a raycast from the position of the camera, and if so, I either break that block or place a block against the corresponding face.

## Procedural Terrain
> Author: Darian Parks

I first had to define functions for grassHeight and mountainHeight in Terrain. grassHeight used voronoi checkpoints, and mountainHeight used Perlin noise.

Once these functions were made, I edited CreateTestScene to instead generate terrain based off of the biome of the coordinate. Biome blending was implemnted via perlin noise and softstepping to make jumping between the two biomes more natural.

Once the biome was determined, I set the block types based on the HW specifications. This also involved adding the SNOW blocktype to the chunk class.

The main difficulty was choosing suffiicent amplitude and frequency values for the height functions, which is still a work in progress up to the final milestone.

# Mini Minecraft Milestone 2
## Multithreaded Terrain Generation
> Author: Bo Sun

To correctly implement terrain expansion, the procedural terrain generation from milestone 1 was first fixed to dynamically extend to newly generated chunks rather than only generating the fixed portion in the test scene. This was done by moving the generation from the `CreateTestScene()` function to the `InstantiateChunkAt()` function while changing noise sampling so that it varied based on x- and z-coordinates of the chunk.

The function `InstantiateZoneAt()` was added, which instantiates an entire zone by looping through the 4-by-4 chunk area and calling `InstantiateChunkAt()` for all unloaded chunks. This also inserts the key into the `m_generatedTerrain` set to mark this zone as generated.

Two new classes, `VBOWorker` and `BlockTypeWorker`, were added as lambda callables that can be used to instantiate threads. `VBOWorker` takes in a pointer to a `Chunk` and its `operator()()` function simply calls the `createVBOdata()` method for that `Chunk`. `BlockTypeWorker` takes in a pointer to `Terrain` and a `glm::ivec2` to represent the x- and z-coordinates of the zone to instantiate, and its `operator()()` function simply calls `instantiateZoneAt()`.

The `loadChunks()` function was significantly changed to support multithreading using the two lambda classes mentioned above. A vector of `std::thread` objects is instantiated, and holds all created threads until the end of the function, at which time the main thread calls `join()` on all of them.

In the `Chunk` class, the `createVBOdata()` function was split into two halves, one of which creates the VBO data and stores it in two vectors, `m_vertexData` and `m_indexData`, and the second of which binds these vectors to the buffers. This was necessary because only the main thread can communicate with the GPU while the child threads were the ones calculating the VBOs, so the two halves could not be contained in a single method. A new block type `UNLOADED` was added to represent blocks that are not in the rendering distance of the player. This allows faces bordering neighboring chunks to be culled even when the neighboring chunk is not yet instantiated, improving the gameplay smoothness. In addition, faces at y=0 were also culled, since the assumption is that the player character cannot survive below the bottom of the map (in the void) and therefore will never see any such face.

Throughout the `Terrain` class, mutex locks were added for both `m_chunks` and `m_generatedTerrain` to prevent race conditions and deadlock.

## Texturing and Texture Animation
> Author: Alex Freeman

In order to load images in as textures to the program, I created a texture class with a create, load, and bind function. The create function creates a new QImage given the path of the image file, and then generated a texture handle. The bind function activates and binds the current texture to the texture slot, and the load function sets the texture parameters and calls glTexImage2D. 

Then I created the sampler2D uniform "u_Texture", which I passed into the lambert fragment shader. In the lambert fragment shader I call the glsl texture function and set that to the base color before the added lighting effects.

I also created a float uniform "u_Time" which is incremented in the myGl tick() function which is passed into the lambert fragment shader. The time offset value is added to th ex value of the uv coordintate whenever uv[2] is 1, which I use as a flag for block types that need animation. This creates the moving effect in the water

Finally, I split the createVbo function into 2 functions, 1 to create transparent vbo data and the other to create opaque vbo data. In the terrain draw() function I loop through all the chunks and create and draw the data for the opaque blocks, using the drawInterleaved() function with a flag for transparency. Then I so the same for the transparent data. I also enabled alpha blending so the water blocks appear as transparent.

## Cave Systems
> Author: Darian Parks

I implemented cave systems in the way described in class; I defined random3 (return a randomized vec3), surflet3d(returned a perlin surflet for a 3d position), and 
perlinNoiseCave(3d perlin noise, but save the surflets to perform trilinear interpolation on). I then added the LAVA and BEDROCK block types and updated the chunk builder 
in accordance with the project outline. 

Additionally, I implemented movement through lava/water by adding an inLiquid field to player, updating the field in Player::computePhysics, and adjusting acceleration 
accordingly in processInputs when inLiquid is true, including a constant swim rate when holding the spacebar.

I unfortunately didn't finish the post processing pipeline by the exact deadline, but I plan on getting it fixed within the next couple of days in office hours.
Additionally, I changed grassHeight to a more simplified version until I put more research in proper worley noise integration, and I increased the change of amplitude 
and added more octaves in the mountainHeight function for more prominent peaks.

The main difficulty was choosing suffiicent amplitude and frequency values for the height functions, which is still a work in progress up to the final milestone.

# Mini Minecraft Milestone 3
## Distance Fog
> Author: Bo Sun

To implement a distance fog, I simply defined an extra uniform variable called `u_Player`, which contains the coordinates of the player. Then, in the fragment shader, I use the fragment's distance to the player to calculate a `fogFactor` and use it to blend the original output color with the fog color.

The fog color was originally a static white-ish hue, but after my implementation of the dynamic sky, it looked strange during sunrise, sunset, and nighttime. Hence, I changed the fog color to be dynamic depending on the time of day. This was done by defining four different colors of fog for morning, noon, afternoon, and night. Using the height of the sun to represent the time of day, I could blend these colors together to determine the correct fog color for that instant, and apply it to the fragment.

## Dynamic Sky
> Author: Bo Sun

To implement a dynamic sky, I began by importing the raycast sky code from class, linking the shader files with the appropriate functions in the `mygl.cpp` file. This was slightly different than what had been done in class, because the structure of the minecraft project was different than the template code.

However, I noticed that the sky looked weird during daytime, because the sky around the sun was constantly a orange-ish hue, which isn't realistic. Therefore, I changed the shader so that for the majority of daytime, when the sun is far away from the horizon, the sky's output color is dominated by a light blue. During the majority of nighttime, when the sun is nowhere to be seen, the sky's output color is dominated by a blue-ish black. This was achieved by adding two extra colors and appropriately using `smoothStep` and `mix` to blend them with the existing colors for sunset and dusk.

I also added some blending around the sun during daytime so that it does not blind the player as much when they look up at it.

## Sound
> Author: Alex Freeman

In order to add sound to the project I used the QMediaPlayer and QAudioOutput classes. First I had to add multimedia to the .pro file. I also created a resource file called sounds which contains an mp3 of each sound effect. For each sound I created a QMediaPlayer and QAudioOutput object, set the Audio output of the media player to the audio output object, set the source using setSource and QUrl, and if necessary set infinite loops and volume settings. I added sounds for background music, placing a block, running, and splashing into water.

## Water Post-Process Effects
> Author: Alex Freeman

In order to create post process effects I used the framebuffer class with an framebuffer called postProcessFrameBuffer. I also used a shaderprogram called progPostProcessWater. The shader and framebuffer are set up in Mygl (mainly paintGl). The drawing is done with a quad drawable which was taken from milestone 2. I also added onto the milestone 2 water glsl file to take in more variables: float for time (u_TIme), int for whether we are in water (u_InLiquid), and the texture (u_Texture). The shader now utilize a random function and a noise function to create a rippling effect, and also uses mix to create a blue tint.

## Procedurally Placed Assets
> Author: Alex Freeman

## Additional Biomes + Temperature and Moisture
> Author: Darian Parks

First off, I further refinded the grassheight function to better resemble rolling hills by using low frequency at high ampltiudes and high frequency at low amplitudes. 
After that, I made three new biomes + height functions. The desert biome is composed of sand and is designed to be the flattest area. The snow biome is composed of snow 
and puts ice on top of water. It is designed as a less severe version of the mountain biome in terms of height but still has noticeable snowy peaks. The mushroom biome 
places mycleium mini islands surrounded by water, vaguely resmebling the mushroom biome from minecraft.

I differentiated between the biomes using temperature and moisture. Desert has high temp, low moisture. Snow has low temp, low moisture. Mountain has low temp, high moisture.
Mushroom has high temp, high moisture. Grassland is more prevelant when both are average. I used a series of smoothsteps in the finalHeight function in order to have more gradual 
transistions between different combinations of biomes. Although I tried to make a more complex system initially, for time constraints I made temperature and moisture correlate more 
with x/z position respectively.

Regretably, the main thing I couldn't refine were the cavesystems, as running the 3d perlin noise function a lot had large performance issues even with chunk multithreading, so it was 
hard to properly test.




