#include <iostream>
#include <string_view>
#include <dino/dino.hpp>

int main(int argc, char const* argv[]) {
	if (argc < 2) { return 1; }
	dino::on_err = [](std::string_view err) { std::cerr << err << '\n'; };
	dino::lib lib("dinolib", argv[1]);
	std::cout << "[ " << lib.name().full_name() << " ] active: " << std::boolalpha << lib.active() << '\n';
	if (!lib) { return 1; }
	if (auto dino_sum = lib.find<int(int, int)>("dino_sum")) { std::cout << "dino_sum(1, 2): " << dino_sum(1, 2) << '\n'; }
	if (auto dino_print = lib.find<void(std::string_view)>("dino_print")) { dino_print("dino wurx!"); }
}
