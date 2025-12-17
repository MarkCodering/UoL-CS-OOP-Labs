#include <iostream>

int main() {
    while (true) {
        // Print the menu options
        std::cout << "\n===== USER MENU =====" << std::endl;
        std::cout << "1: Print help" << std::endl;
        std::cout << "2: Print exchange stats" << std::endl;
        std::cout << "3: Place an ask" << std::endl;
        std::cout << "4: Place a bid" << std::endl;
        std::cout << "5: Print wallet" << std::endl;
        std::cout << "6: Continue" << std::endl;
        std::cout << "0: Exit" << std::endl;

        // Read user input
        int userOption;
        std::cout << "Type in your choice (0–6): ";
        std::cin >> userOption;

        // Input validation
        if (std::cin.fail()) {
            std::cin.clear(); // clear the error flag
            std::cin.ignore(1000, '\n'); // discard invalid input
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        // Process user input using switch
        switch (userOption) {
            case 1:
                std::cout << "\nHelp - choose options from the menu and follow the on-screen instructions." << std::endl;
                break;
            case 2:
                std::cout << "\nExchange stats: Market volume is stable today." << std::endl;
                break;
            case 3:
                std::cout << "\nPlace an ask - please specify the price and amount." << std::endl;
                break;
            case 4:
                std::cout << "\nPlace a bid - please specify your offer details." << std::endl;
                break;
            case 5:
                std::cout << "\nWallet balance: 5 BTC, 1000 USD." << std::endl;
                break;
            case 6:
                std::cout << "\nContinuing to the next step..." << std::endl;
                break;
            case 0:
                std::cout << "\nExiting program. Goodbye!" << std::endl;
                return 0;
            default:
                std::cout << "\nInvalid choice. Please select a number between 0 and 6." << std::endl;
                break;
        }
    }

    return 0;
}
