#include "Observable.h"
#include "state.h"
#include <iostream>
#include <unistd.h>

using namespace state;
using namespace engine;
using namespace std;

//mutable std::vector<Observer*> observers;

void engine::Observable::registerObserver(Observer* o){
	observers.push_back(o);
}

void engine::Observable::notifyUpdating(){
	for(auto observer : observers){
		observer->engineUpdating();
	}
}

void engine::Observable::notifyUpdated(){
	for(auto observer : observers){
		observer->engineUpdated();
	}
}
