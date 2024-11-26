#include <iostream>
#include <vector>
#include<Windows.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;


void MySetColor(int fg, int bg) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, fg | (bg << 4));
}


class Exception {
	string text;
	int line;
public:
	Exception(string text, int line) : text(text), line(line) {}
	friend ostream& operator<<(ostream& out, const Exception& ex) {
		out << ex.text << " Line: " << ex.line << endl;
		return out;
	}
};

class Ingredient {
public:
	string name;
	double quantity;
	double price_per_kg;

	Ingredient(string name, double quantity, double price) : name(name), quantity(quantity), price_per_kg(price) {
		if (quantity < 0 || price_per_kg < 0) {
			throw Exception("Quantity and price per kg must be non-negative.", __LINE__);
		}
	}

	void update(const string& newName, double newQuantity, double newPricePerKg) { 
		if (newQuantity < 0 || newPricePerKg < 0) { 
			throw Exception("Quantity and price per kg must be non-negative.", __LINE__); 
		} 
		name = newName; 
		quantity = newQuantity; 
		price_per_kg = newPricePerKg; 
		write_to_file("data.txt");
	}
	void write_to_file(const string& filename) const { 
		ofstream file(filename, ios::app); 
		if (file.is_open()) { 
			file << name << " " << quantity << " " << price_per_kg << endl;
			file.close(); 
		} 
		else {
			cout << "Unable to open file for writing.\n";
		} 
	}
};

class Meal {
public:
	string name;
	vector<Ingredient> ingredients;
	string additional_info;
	double price;

	Meal(string name, vector<Ingredient> ings, string info, double price)
		: name(name), ingredients(ings), additional_info(info), price(price) {
		if (price < 0) {
			throw Exception("Price must be non-negative.", __LINE__);
		}
	}

	void update(const string& newName, const vector<Ingredient>& newIngredients, const string& newInfo, double newPrice, const string& filename) { 
		if (newPrice < 0) {
			throw Exception("Price must be non-negative.", __LINE__);
		}
		name = newName;
		ingredients = newIngredients; 
		additional_info = newInfo; 
		price = newPrice; 
		write_to_file("meals.txt");
	}
	void write_to_file(const string& filename) const { 
		ofstream file(filename, ios::app); 
		if (file.is_open()) {
			file << name << "\n" << additional_info << "\n" << price << endl; 
			for (const auto& ing : ingredients) 
			{ 
				file << ing.name << " " << ing.quantity << " " << ing.price_per_kg << endl; 
			}
			file << "END\n"; 
			file.close(); 
		} 
		else { 
			cout << "Unable to open file for writing.\n";
		}
	}
};


class User {
public:
	string username;
	string password;
	string role;
	vector<Meal> basket;

	User(string username, string password, string role) : username(username), password(password), role(role) {}

	void addToBasket(const Meal& meal) {
		basket.push_back(meal);
	}

	void removeFromBasket(const string& mealName) {
		for (auto i = basket.begin(); i != basket.end(); ++i) {
			if (i->name == mealName) {
				basket.erase(i);
				break;
			}
		}
	}

	void displayBasket() const {
		cout << "Baket:\n";
		for (const auto& meal : basket) {
			cout << "Meal: " << meal.name << ", Info: " << meal.additional_info << ", Price: $" << meal.price << endl;
		}
	}

	void writeToUserFile(const string& filename) const {
		ofstream file(filename, ios::app);
		if (file.is_open()) {
			file << username << " " << password << " " << role << endl;
			file.close();
		}
		else {
			cout << "Unable to open file for writing.\n";
		}
	}

	static void readUserFile(vector<User>& users, const string& filename) {
		ifstream file(filename);
		if (file.is_open()) {
			users.clear();
			string line;
			while (getline(file, line)) {
				istringstream iss(line);
				string username, password, role;
				if (iss >> username >> password >> role) {
					users.emplace_back(username, password, role);
				}
			}
			file.close();
			cout << "Data read from file successfully.\n";
		}
		else {
			cout << "Unable to open file for reading.\n";
		}
	}

	static bool signIn(vector<User>& users, const string& filename) {
		string username, password, role;
		cout << "Enter username: ";
		cin >> username;
		cout << "Enter password: ";
		cin >> password;
		cout << "Enter role (admin/user): ";
		cin >> role;

		for (const auto& user : users) {
			if (user.username == username) {
				cout << "Username already exists. Please try another one.\n";
				return false;
			}
		}

		User newUser(username, password, role);
		users.push_back(newUser);
		newUser.writeToUserFile(filename);
		cout << "Registration successful!\n";
		return true;
	}

	static User* login(vector<User>& users) {
		string username, password;
		cout << "Enter username: ";
		cin >> username;
		cout << "Enter password: ";
		cin >> password;

		for (auto& user : users) {
			if (user.username == username && user.password == password) {
				cout << "Login successful!\n";
				return &user;
			}
		}
		cout << "Invalid username or password. Please try again.\n";
		return nullptr;
	}
};
class AdminPanel {
private:
	vector<Ingredient> stock;
	vector<Meal> menu;
	double budget;

public:
	AdminPanel(double initial_budget) : budget(initial_budget) {}

	void addIngredientToStock(const Ingredient& ing) {
		if (ing.quantity < 0 || ing.price_per_kg < 0) {
			throw Exception("Quantity and price per kg must be non-negative.", __LINE__);
		}

		bool found = false;
		for (auto& s : stock) {
			if (s.name == ing.name) {
				s.quantity += ing.quantity;
				found = true;
				break;
			}
		}
		if (!found) {
			stock.push_back(ing);
		}
		budget -= ing.quantity * ing.price_per_kg;
		Write_to_File("data.txt");
	}

	void addMealToMenu(const Meal& meal) {
		if (meal.price < 0) {
			throw Exception("Price must be non-negative.", __LINE__);
		}

		menu.push_back(meal);
		Write_to_File("data.txt");
		writeMealToFile("meals.txt", meal);
	}

	void processOrder(const string& mealName) {
		for (const auto& meal : menu) {
			if (meal.name == mealName) {
				for (const auto& ing : meal.ingredients) {
					for (auto& s : stock) {
						if (s.name == ing.name) {
							s.quantity -= ing.quantity;
						}
					}
				}
				break;
			}
		}
		Write_to_File("data.txt");
	}

	bool isMealAvailable(const Meal& meal) const {
		for (const auto& ing : meal.ingredients) {
			for (const auto& stockItem : stock) {
				if (stockItem.name == ing.name) {
					if (stockItem.quantity < ing.quantity) {
						return false; // Yeterli stok yoxdur
					}
					break;
				}
			}
		}
		return true; // Bütün inqredientlər yetərlidir
	}

	void deleteIngredient(const string& ingName) {
		for (auto it = stock.begin(); it != stock.end(); ++it) {
			if (it->name == ingName) {
				stock.erase(it);
				Write_to_File("data.txt");
				cout << "Ingredient " << ingName << " deleted successfully.\n";
				return;
			}
		}
		cout << "Ingredient " << ingName << " not found in stock.\n";
	}

	void deleteMeal(const string& mealName) {
		for (auto it = menu.begin(); it != menu.end(); ++it) {
			if (it->name == mealName) {
				menu.erase(it);
				writeMealsToFile("meals.txt");
				cout << "Meal " << mealName << " deleted successfully.\n";
				return;
			}
		}
		cout << "Meal " << mealName << " not found in menu.\n";
	}

	void writeMealsToFile(const string& filename) const {
		ofstream file(filename);
		if (file.is_open()) {
			for (const auto& meal : menu) {
				file << meal.name << "\n" << meal.additional_info << "\n" << meal.price << endl;
				for (const auto& ing : meal.ingredients) {
					file << ing.name << " " << ing.quantity << " " << ing.price_per_kg << endl;
				}
				file << "END\n";
			}
			file.close();
			cout << "Menu data written to file successfully.\n";
		}
		else {
			cout << "Unable to open file for writing.\n";
		}
	}

	void displayStock() const {
		cout << "Stock:\n";
		for (const auto& s : stock) {
			cout << "Ingredient: " << s.name << ", Quantity: " << s.quantity << " kg\n";
		}
	}

	void displayBudget() const {
		cout << "Current Budget: $" << budget << "\n";
	}

	void displayMenu() {
		menu.clear();
		readMealsFromFile("meals.txt");

		cout << "Menu:\n";
		for (const auto& meal : menu) {
			cout << "Meal: " << meal.name << ", Info: " << meal.additional_info << ", Price: $" << meal.price << endl;
		}
	}

	void Write_to_File(const string& filename) const {
		ofstream file(filename);
		if (file.is_open()) {
			file << budget << endl;
			for (const auto& s : stock) {
				file << s.name << " " << s.quantity << " " << s.price_per_kg << endl;
			}
			file.close();
			cout << "Stock data written to file successfully.\n";
		}
		else {
			cout << "Unable to open file for writing.\n";
		}
	}

	void ReadFile(const string& filename) {
		ifstream file(filename);
		if (file.is_open()) {
			file >> budget;
			stock.clear();
			string name;
			double quantity, price_per_kg;
			while (file >> name >> quantity >> price_per_kg) {
				stock.push_back(Ingredient(name, quantity, price_per_kg));
			}
			file.close();
			cout << "Stock data read from file successfully.\n";
		}
		else {
			cout << "Unable to open file for reading.\n";
		}
	}

	vector<Meal> getMenu() const { return menu; }
	vector<Ingredient> getStock() const { return stock; }

	void writeMealToFile(const string& filename, const Meal& meal) const {
		ofstream file(filename, ios::app);
		if (file.is_open()) {
			file << meal.name << "\n" << meal.additional_info << "\n" << meal.price << endl;
			for (const auto& ing : meal.ingredients) {
				file << ing.name << " " << ing.quantity << " " << ing.price_per_kg << endl;
			}
			file << "END\n";
			file.close();
		}
		else {
			cout << "Unable to open file for writing.\n";
		}
	}

	void readMealsFromFile(const string& filename) {
		ifstream file(filename);
		if (file.is_open()) {
			menu.clear();
			string mealName, info, line;
			double price;
			while (getline(file, mealName)) {
				if (mealName.empty()) continue; // Boş sətirləri ötür
				getline(file, info);
				file >> price; // Qiyməti oxu
				file.ignore(); // Satırı tam oxumaq üçün istifadə olundu
				vector<Ingredient> ingredients;
				string ingName;
				double quantity, price_per_kg;
				while (getline(file, line)) {
					if (line == "END") break;
					istringstream iss(line);
					iss >> ingName >> quantity >> price_per_kg;
					ingredients.push_back(Ingredient(ingName, quantity, price_per_kg));
				}
				menu.push_back(Meal(mealName, ingredients, info, price));
			}
			file.close();
		}
		else {
			cout << "Unable";
		}
	}
};

void clearScreen() {
	system("cls");
}


void adminPanel(AdminPanel& admin);
void userPanel(User& user, AdminPanel& admin);

int mainMenu(AdminPanel& admin, vector<User>& users) {
	system("cls");
	while (true) {
		int option;
		MySetColor(9,0);
		cout << "Xos Gelmisiniz!" << endl;
		cout << "1. Sign In\n2. Login\n3. Exit\nEnter your choice: ";
		cin >> option;

		User* loggedInUser = nullptr;

		if (option == 1) {
			User::signIn(users, "users.txt");
		}
		else if (option == 2) {
			loggedInUser = User::login(users);
		}
		else if (option == 3) {
			cout << "Exiting program...\n";
			return 0;
		}
		else {
			cout << "Invalid choice. Please choose a valid option.\n";
			continue;
		}

		if (loggedInUser) {
			if (loggedInUser->role == "admin") {
				adminPanel(admin);
			}
			else if (loggedInUser->role == "user") {
				admin.ReadFile("data.txt");
				userPanel(*loggedInUser, admin);
			}
			else {
				cout << "Invalid role. Please restart the program and enter 'admin' or 'user'.\n";
			}
		}
	}
}

void adminPanel(AdminPanel& admin) {
	system("cls");
	while (true) {
		int choice;
		cout << "Admin Panel" << endl;
		MySetColor(6, 0);
		cout << "Hemise seni gorek Boss\n" << endl;
		cout << "1. Display Menu\n2. Add Meal to Menu\n3. Display Stock\n4. Add Ingredient to Stock\n5. Display Budget\n6. Delete Ingredient\n7. Delete Meal\n8. Exit\n9. Back to Main Menu\nEnter your choice:";
		cin >> choice;
		switch (choice) {
		case 1:
			admin.displayMenu();
			break;
		case 2: {
			string mealName, info;
			double price;
			int numIngredients;
			vector<Ingredient> ingredients;

			cout << "Enter meal name: ";
			cin.ignore();
			getline(cin, mealName);
			cout << "Enter additional info: ";
			getline(cin, info);
			cout << "Enter price: "; 
			cin >> price;
			cout << "Enter number of ingredients: ";
			cin >> numIngredients;

			cout << "Available Ingredients:\n";
			for (const auto& s : admin.getStock()) {
				cout << "Ingredient: " << s.name << ", Quantity: " << s.quantity << " kg, Price per kg: " << s.price_per_kg << endl;
			}

			for (int i = 0; i < numIngredients; ++i) {
				string ingName;
				double quantity, price_per_kg = 0.0;
				cout << "Ingredient " << (i + 1) << " name: ";
				cin.ignore();
				getline(cin, ingName);
				cout << "Quantity (kg): ";
				cin >> quantity;

				for (auto& s : admin.getStock()) {
					if (s.name == ingName) {
						price_per_kg = s.price_per_kg;
						s.quantity -= quantity;
						break;
					}
				}

				ingredients.emplace_back(ingName, quantity, price_per_kg);
			}
			admin.addMealToMenu(Meal(mealName, ingredients, info, price));
			admin.Write_to_File("data.txt"); 
			break;
		}
		case 3:
			admin.displayStock();
			break;
		case 4: {
			string ingName;
			double quantity, price_per_kg;
			cout << "Enter ingredient name: ";
			cin.ignore();
			getline(cin, ingName);
			cout << "Enter quantity (kg): ";
			cin >> quantity;
			cout << "Enter price per kg: ";
			cin >> price_per_kg;
			admin.addIngredientToStock(Ingredient(ingName, quantity, price_per_kg));
			break;
		}
		case 5:
			admin.displayBudget();
			break;
		case 6: {
			string ingName;
			cout << "Enter the name of the ingredient to delete: ";
			cin.ignore();
			getline(cin, ingName);
			admin.deleteIngredient(ingName);
			break;
		}
		case 7: {
			string mealName;
			cout << "Enter the name of the meal to delete: ";
			cin.ignore();
			getline(cin, mealName);
			admin.deleteMeal(mealName);
			break;
		}
		case 8:
			cout << "Exiting admin panel...\n";
		case 9:
			return;
		default:
			cout << "Invalid choice, please try again.\n";
			break;
		}
	}
}


void userPanel(User& user, AdminPanel& admin) {
	system("cls");
	while (true) {
		int choice;
		cout << "User Panel" << endl;
		MySetColor(2, 0);
		cout << "\nRestoranimiza Xos Gelmisiniz!\n";
		cout << "1. Display Menu\n2. Add Meal to Basket\n3. Display Basket\n4. Place Order\n5. Exit\n6. Back to Main Menu\nEnter your choice: ";
		cin >> choice;
		switch (choice) {
		case 1:
			admin.displayMenu();
			break;
		case 2: {
			string mealName;
			cout << "Enter the name of the meal to add to Basket: ";
			cin >> mealName;
			bool found = false;
			for (const auto& meal : admin.getMenu()) {
				if (meal.name == mealName) {
					if (admin.isMealAvailable(meal)) {
						user.addToBasket(meal);
						cout << "Meal added to basket.\n";
					}
					else {
						cout << "Sorry, there is not enough ingredients in stock to make this meal.\n";
					}
					found = true;
					break;
				}
			}
			if (!found) {
				cout << "Meal not found in the menu.\n";
			}
			break;
		}
		case 3:
			user.displayBasket();
			break;
		case 4:
			if (!user.basket.empty()) {
				for (const auto& meal : user.basket) {
					if (admin.isMealAvailable(meal)) {
						admin.processOrder(meal.name);
						user.basket.clear();
						cout << "Order placed successfully!\n";
					}
					else {
						cout << "Sorry, there is not enough ingredients in stock for some of the meals in your basket.\n";
					}
				}
			}
			else {
				cout << "Your basket is empty.\n";
			}
			break;
		case 5:
			cout << "Exiting user panel...\n";
			exit(0);
		case 6:
			return; // Geri qayıtma (Back to Main Menu) funksiyası
		default:
			cout << "Invalid choice, please try again.\n";
			break;
		}
	}
}

void main() {
	try
	{
		AdminPanel admin(1000.0);
		admin.ReadFile("data.txt");

		vector<User> users;
		User::readUserFile(users, "users.txt");

		while (true) {
			if (mainMenu(admin, users) == 0) {
				break;
			}
		}
	}
	catch (const Exception& x)
	{
		cout << "Error: " << x << endl;
	}
	

}
