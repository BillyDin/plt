
#include <boost/test/unit_test.hpp>
#include "../../src/client/client.h"
#include <SFML/Graphics.hpp>
using namespace ::client;
BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
	BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestSFML)
{
	{
		::sf::Texture texture;
		BOOST_CHECK(texture.getSize() == ::sf::Vector2<unsigned int>{});
	}
}

BOOST_AUTO_TEST_CASE(TestClientNamespace)
{
	{
		engine::Engine e{"engine"};
		e.getState().initializeCharacters();
		e.getState().initializeMapCell();
		KeyboardListener::triggerAction(e, SELECT);
		e.getState().setActualAction(state::MOVING);
		KeyboardListener::triggerAction(e, MOVE);
		KeyboardListener::triggerAction(e, ATTACK);
		KeyboardListener::triggerAction(e, PASS_TURN);
	}
}

/* vim: set sw=2 sts=2 et : */
