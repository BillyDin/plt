
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
		e.getState().initializeMapCell("../../../res/map_v0.txt");
		e.getState().getCursor().setPosition(e.getState().getCharacters()[0]->getPosition());
		KeyboardListener kl{e};
		kl.triggerAction(e, SELECT);
		kl.triggerAction(e, MOVE);
		kl.triggerAction(e, ATTACK);
		kl.triggerAction(e, PASS_TURN);
	}
}

/* vim: set sw=2 sts=2 et : */
