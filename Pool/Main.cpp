#include "Pool.hpp"

static const std::string LUA_SCENE = "pool.lua";
static const std::string TITLE = "Billiards";

int main( int argc, char **argv ) 
{
	CS488Window::launch(argc, argv, new Pool(LUA_SCENE), 1024, 768, TITLE);
	return 0;
}
