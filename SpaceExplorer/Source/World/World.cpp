#include "World.h"

//Private functions:
uint8_t getValue(sf::Image im, int x, int y)
{
	x %= im.getSize().x;
	y %= im.getSize().y;

	return im.getPixel(x, y).r;

}

void setValue(sf::Image* im, int x, int y, uint8_t v)
{
	x %= im->getSize().x;
	y %= im->getSize().y;

	im->setPixel(x, y, sf::Color(v, v, v));
}

void setValue(sf::Image* im, int x, int y, sf::Color c)
{
	x %= im->getSize().x;
	y %= im->getSize().y;

	im->setPixel(x, y, c);
}

//Get all pixels more than 0 arround point
int getSurrounding(sf::Image im, int x, int y, int r = 2)
{
	x %= im.getSize().x;
	y %= im.getSize().y;

	int c = 0;

	for (int ox = -r / 2; ox < r / 2; ox++)
	{
		for (int oy = -r / 2; oy < r / 2; oy++)
		{
			if (im.getPixel((x + ox) % im.getSize().x, (y + oy) % im.getSize().y).r >= 1)
			{
				c++;
			}
		}
	}

	return c;
}


//------------------------------------------------------------------

void World::generate()
{
	srand(seed);


	sf::Image elev = sf::Image();
	elev.create(width, height, sf::Color::White);

	//Generate test map
	noise::module::Perlin perlin;
	noise::module::RidgedMulti ridged;
	noise::module::Perlin perlin2;

	perlin.SetSeed(seed);
	ridged.SetSeed(seed);
	perlin2.SetSeed(seed);

	perlin.SetFrequency(islands);

	perlin2.SetFrequency(islands * 2);
	perlin2.SetLacunarity(1.5);

	ridged.SetFrequency(islands * 5);


	for (int x = 0; x < width / 2 - 6; x++)
	{
		for (int y = 0; y < height; y++)
		{
			//Generate half the map
			double dX = (double)x / detail;
			double dY = (double)y / detail;

			double valPd = perlin.GetValue(dX, dY, 12.0);
			unsigned int valP = ((valPd + 1) / 2) * 255;

			double valRd = ridged.GetValue(dX, dY, 5.0);
			unsigned int valR = ((valRd + 1) / 2) * 255;

			double valPPd = perlin2.GetValue(dX + seed, dY + seed, 54.0);
			unsigned int valPP = ((valPPd + 1) / 2) * 255;

			unsigned int val = (valPP + valP + valR) / 3;



			if (val > seaLevel * 1.2)
			{
				unsigned int pval = val;
				val += 32 * (valRd * mountains);
				val %= 255;
				if (pval > val)
				{
					val = pval;
				}
			}

			if (val < seaLevel)
			{
				val += valRd * connection;
				val %= 255;
			}


			if (val < seaLevel)
			{
				val = 0;
			}

			if (y <= (polarCap + rand() % polarCapRand - (polarCapRand / 2)) ||
				y >= height - (polarCap + rand() % polarCapRand - (polarCapRand / 2)))
			{
				val = seaLevel + 20;
			}

			elev.setPixel(x, y, sf::Color(val, val, val, 255));
		}
	}

	//Generate other half
	for (int x = width / 2 - 6; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			//Mirror the other half in the x axis
			unsigned int val = elev.getPixel(width - x, y).r;

			//Generate half the map
			double dX = (double)x / detail;
			double dY = (double)y / detail;

			double valPd = perlin.GetValue(dX, dY, 12.0);
			unsigned int valP = ((valPd + 1) / 2) * 255;

			double valRd = ridged.GetValue(dX, dY, 5.0);
			unsigned int valR = ((valRd + 1) / 2) * 255;

			double valPPd = perlin2.GetValue(dX + seed, dY + seed, 54.0);
			unsigned int valPP = ((valPPd + 1) / 2) * 255;

			val = (valPP + valP + valR) / 4;

			if (val > seaLevel * 1.2)
			{
				unsigned int pval = val;
				val += 32 * (valRd * mountains);
				val %= 255;
				if (pval > val)
				{
					val = pval;
				}
			}

			if (val < seaLevel)
			{
				val += valRd * connection;
				val %= 255;
			}


			if (val < seaLevel)
			{
				val = 0;
			}

			if (y <= (polarCap + rand() % polarCapRand - (polarCapRand / 2)) ||
				y >= height - (polarCap + rand() % polarCapRand - (polarCapRand / 2)))
			{
				val = seaLevel + 20;
			}

			elev.setPixel(x, y, sf::Color(val, val, val));
		}
	}

	seed += 512;
	perlin.SetSeed(seed);
	ridged.SetSeed(seed);
	perlin2.SetSeed(seed);



	//Now globally modify some stuff to REDUCE (TODO) the wrap-arounds
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			//Generate half the map
			unsigned int val = elev.getPixel(width - x - 1, y).r;

			double dX = (double)x / detail;
			double dY = (double)y / detail;

			double valPd = perlin.GetValue(dX, dY, 12.0);
			unsigned int valP = ((valPd + 1) / 2) * 255;

			double valRd = ridged.GetValue(dX, dY, 5.0);
			unsigned int valR = ((valRd + 1) / 2) * 255;

			double valPPd = perlin2.GetValue(dX + seed, dY + seed, 54.0);
			unsigned int valPP = ((valPPd + 1) / 2) * 255;

			val = (valPP + valP + valR + val) / 4 * 1.3;



			if (val > seaLevel)
			{
				unsigned int pval = val;
				val += 32 * (valRd * mountains);
				val %= 255;
				if (pval > val)
				{
					val = pval;
				}
			}

			if (val < seaLevel)
			{
				val += valRd * connection;
				val %= 255;
			}


			if (val < seaLevel)
			{
				val = 0;
			}

			if (y <= (polarCap + rand() % polarCapRand - (polarCapRand / 2)) ||
				y >= height - (polarCap + rand() % polarCapRand - (polarCapRand / 2)))
			{
				val = seaLevel + 20;
			}

			elev.setPixel(x, y, sf::Color(val, val, val, 255));
		}
	}

	//Invert?
	if (invert)
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				uint8_t oldVal = elev.getPixel(x, y).r;
				if (oldVal == 0)
				{
					oldVal += (perlin.GetValue(x / 10, y / 10, 53.3) * 120);
				}
				elev.setPixel(x, y, sf::Color(255 - oldVal, 255 - oldVal, 255 - oldVal));
			}
		}

		//Now globally modify some stuff to REDUCE the cuts
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				//Generate half the map
				unsigned int val = elev.getPixel(width - x - 1, y).r;

				double dX = (double)x / detail;
				double dY = (double)y / detail;

				double valPd = perlin.GetValue(dX, dY, 12.0);
				unsigned int valP = ((valPd + 1) / 2) * 255;

				double valRd = ridged.GetValue(dX, dY, 5.0);
				unsigned int valR = ((valRd + 1) / 2) * 255;

				double valPPd = perlin2.GetValue(dX + seed, dY + seed, 54.0);
				unsigned int valPP = ((valPPd + 1) / 2) * 255;

				val = (valPP + valP + valR + val) / 4 * 1.3;



				if (val > seaLevel)
				{
					unsigned int pval = val;
					val += 32 * (valRd * mountains);
					val %= 255;
					if (pval > val)
					{
						val = pval;
					}
				}

				if (val < seaLevel)
				{
					val += valRd * connection;
					val %= 255;
				}


				if (val < seaLevel)
				{
					val = 0;
				}

				if (y <= (polarCap + rand() % polarCapRand - (polarCapRand / 2)) ||
					y >= height - (polarCap + rand() % polarCapRand - (polarCapRand / 2)))
				{
					val = seaLevel + 20;
				}

				val = (elev.getPixel(x, y).r * 2 + val) / 4;

				elev.setPixel(x, y, sf::Color(val, val, val, 255));
			}
		}

	}

	//Climate generation

	sf::Image clim = sf::Image();
	clim.create(width, height);

	ridged.SetFrequency(mountains);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{

			unsigned int val = 0;
			if (y > height / 2)
			{
				val = height - y;
			}
			else
			{
				val = y;
			}

			val *= 1.9;

			clim.setPixel(x, y, sf::Color(val, val, val, 255));
		}
	}

	//Add some noise
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{

			unsigned int val = clim.getPixel(x, y).r;
			double dval = perlin.GetValue(x, y, 24.53) * 30.0;
			val = (val + dval) / 2;
			clim.setPixel(x, y, sf::Color(val, val, val, 255));
		}
	}

	//-----------------------------------------------
	//Rainfall

	//Every sea tile creates rainfall, it moves right until it mets a tile
	//higher than 1.4 sealevel

	//Basically, for each x we will check until we meet a square and cut rainfall

	sf::Image rain = sf::Image();
	rain.create(width, height, sf::Color::White);


	for (int y = 0; y < height; y++)
	{
		int count = 0;
		for (int x = 0; x < width; x++)
		{
			if (count <= 0)
			{
				if (elev.getPixel(x, y).r >= seaLevel * 1.4)
				{
					count = 80;
				}
			}
			else
			{
				if (elev.getPixel(x, y).r >= seaLevel * 1.4)
				{
					count += 60;
				}
				count--;
				rain.setPixel(x, y, sf::Color(255 - count, 255 - count, 255 - count));
			}
		}
	}



	//Generate shores & beaches (basic cellular automata)

	sf::Image outim = elev;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{

			//Lone pixels may get deleted
			if (getSurrounding(elev, x, y) <= 1)
			{
				if (rand() % 1000 >= 250)
				{
					elev.setPixel(x, y, sf::Color::Black);
				}
			}

			//If a pixel is not fully surrounded its beach
			if (getSurrounding(elev, x, y) <= 3 && getSurrounding(elev, x, y) >= 1)
			{
				outim.setPixel(x, y, sf::Color(seaLevel + 1, seaLevel + 1, seaLevel + 1));
			}

			//If a pixel is closer than 3 pixels to something then it's shore
			if (getSurrounding(elev, x, y, 4) >= 1 && elev.getPixel(x, y).r <= seaLevel)
			{
				outim.setPixel(x, y, sf::Color(seaLevel - 50, seaLevel - 50, seaLevel - 50));
			}


		}
	}
	elev = outim;


	//Generate biome map
	//BY RGB 
	//128 164 177  -  Ocean (elev <= seaLevel)
	//187 223 237  -  Shore (elev >= 25 && elev <= seaLevel)
	//213 201 160  -  Beach (elev == seaLevel + 1)
	//--------------------------------------------------------
	//130 160 109  -  Grassland (med temp, med rain, med< height)
	//139 196 131  -  Rain (-high temp, good rain, med< height)
	//198 237 198  -  Tundra (low temp, any rain, med< height)
	//201 190 153  -  Desert (>high temp, low rain, med< height)
	//235 235 235  -  Mountain (any temp, any rain, high height)
	//196 220 130  -  Savanna (high temp, low rain, med height)
	//136 191 106  -  Plains (everything else)
	//255 255 255  -  Pole
	//OTHER: Plains

	sf::Image biome = sf::Image();
	biome.create(width, height, sf::Color(136, 191, 106));

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (y <= (polarCap + rand() % polarCapRand - (polarCapRand / 2)) || 
				y >= height - (polarCap + rand() % polarCapRand - (polarCapRand / 2)))
			{
				biome.setPixel(x, y, sf::Color(255, 255, 255));
			}
			else if (elev.getPixel(x, y).r <= seaLevel)
			{
				if (elev.getPixel(x, y).r >= 25)
				{
					//Shore
					biome.setPixel(x, y, sf::Color(187, 223, 237));
				}
				else
				{
					//Ocean
					biome.setPixel(x, y, sf::Color(128, 164, 177));
				}
			}
			else
			{
				if (elev.getPixel(x, y).r == seaLevel + 1)
				{
					//Shore
					biome.setPixel(x, y, sf::Color(213, 201, 160));
				}
				else
				{
					//Forest
					if (elev.getPixel(x, y).r <= 230)
					{
						if (rain.getPixel(x, y).r >= 80 && rain.getPixel(x, y).r <= 210)
						{
							if (clim.getPixel(x, y).r >= 70 && clim.getPixel(x, y).r <= 90)
							{
								biome.setPixel(x, y, sf::Color(130, 160, 109));
							}
						}
						//Jungle
						else if (rain.getPixel(x, y).r >= 140)
						{
							if (clim.getPixel(x, y).r >= 90)
							{
								biome.setPixel(x, y, sf::Color(139, 196, 131));
							}
						}
						else if (rain.getPixel(x, y).r <= 140)
						{
							//Desert
							if (clim.getPixel(x, y).r >= 140)
							{
								biome.setPixel(x, y, sf::Color(201, 190, 153));
							}

							//Savanna
							if (clim.getPixel(x, y).r >= 80)
							{
								biome.setPixel(x, y, sf::Color(196, 220, 130));
							}
						}
					}
					else if (elev.getPixel(x, y).r >= 245)
					{
						//Mountain 100%
						biome.setPixel(x, y, sf::Color(235, 235, 235));
					}

					//Tundra
					if (clim.getPixel(x, y).r <= 20)
					{
						biome.setPixel(x, y, sf::Color(198, 237, 198));
					}


				}
			}
		}
	}



	//Strategic Resource distribution generation
	//FOREST (& ANIMALS): GREEN CHANNEL
	//MINERAL: BLUE CHANNEL
	//PETROL: RED CHANNEL
	//URANIUM: ALPHA CHANNEL
	//If more are to be added create another image

	sf::Image strat = sf::Image();
	strat.create(width, height, sf::Color::Black);


	perlin.SetLacunarity(rand() % 12);
	perlin.SetFrequency(rand() % 12 + 3);

	perlin2.SetLacunarity(rand() % 6);
	perlin2.SetFrequency(rand() % 5 + 2);

	ridged.SetFrequency(rand() % 5 + 2);
	ridged.SetLacunarity(rand() % 10 + 4);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			//FOREST & MINERALS follow perlin distributions
			double dX = (double)x / detail;
			double dY = (double)y / detail;

			double valPd = perlin.GetValue(dX, dY, 12.0);
			unsigned int valP = ((valPd + 1) / 2) * 255;

			double valRd = ridged.GetValue(dX, dY, 5.0);
			unsigned int valR = ((valRd + 1) / 2) * 255;

			double valPPd = perlin2.GetValue(dX + seed, dY + seed, 54.0);
			unsigned int valPP = ((valPPd + 1) / 2) * 255;

			if (elev.getPixel(x, y).r >= seaLevel)
			{

				sf::Color c = sf::Color((valPP + valRd / 2), valP, valPP, 255);

				strat.setPixel(x, y, c);
			}
		}
	}

	this->biome = biome;
	this->elevation = elev;
	this->rainfall = rain;
	this->climate = clim;
	this->resources = strat;

}

World::World()
{

	seed = 4567;
	width = 128;
	height = 128;
	detail = 50.3;
	connection = 12.4;
	mountains = 6.5;
	seaLevel = 150;
	smooth = true;
	islands = 0.55;
	polarCap = 10;
	polarCapRand = 10;
	rivers = 10;

}


World::~World()
{
}