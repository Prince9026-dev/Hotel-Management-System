#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

class Room
{
private:
    int roomNumber;
    string roomType;
    bool isAvailable;
    double pricePerNight;

    static int totalRooms;
    static int availableRoomsCount;

public:
    // Constructor
    Room(int num, string type, double price, bool available = true)
    {
        roomNumber = num;
        roomType = type;
        pricePerNight = price;
        isAvailable = available;

        totalRooms++;
        if (isAvailable)
        {
            availableRoomsCount++;
        }
    }

    // Getters
    int getRoomNumber() const { return roomNumber; }
    string getRoomType() const { return roomType; }
    bool getAvailability() const { return isAvailable; }
    double getPricePerNight() const { return pricePerNight; }

    // Setter to update availability
    void setAvailability(bool status)
    {
        if (isAvailable != status)
        {
            isAvailable = status;
            if (isAvailable)
            {
                availableRoomsCount++;
            }
            else
            {
                availableRoomsCount--;
            }
        }
    }

    static int getTotalRooms() { return totalRooms; }
    static int getAvailableRoomsCount() { return availableRoomsCount; }

    // Reset static counters when reloading from file
    static void resetCounters()
    {
        totalRooms = 0;
        availableRoomsCount = 0;
    }
};

int Room::totalRooms = 0;
int Room::availableRoomsCount = 0;

class User
{
protected:
    string name;
    string phoneNumber;

public:
    void setDetails()
    {
        cout << "Enter Customer Name: ";
        cin >> ws; // Clear input buffer
        getline(cin, name);
        cout << "Enter Phone Number: ";
        cin >> phoneNumber;
    }

    void displayUser() const
    {
        cout << "Customer Name  : " << name << "\n";
        cout << "Phone Number   : " << phoneNumber << "\n";
    }

    string getName() const { return name; }
    string getPhoneNumber() const { return phoneNumber; }

    void setName(const string &n) { name = n; }
    void setPhoneNumber(const string &p) { phoneNumber = p; }
};

class Customer : public User
{
private:
    int bookedRoomNumber;
    int stayDays;

public:
    Customer() : bookedRoomNumber(-1), stayDays(0) {}
    Customer(string n, string phone, int roomNo, int days)
    {
        name = n;
        phoneNumber = phone;
        bookedRoomNumber = roomNo;
        stayDays = days;
    }

    int getBookedRoomNumber() const { return bookedRoomNumber; }
    void setBookedRoomNumber(int roomNo) { bookedRoomNumber = roomNo; }
    void setStayDays(int days) { stayDays = days; }
    int getStayDays() const { return stayDays; }
};

class HotelManager
{
private:
    vector<Room> rooms;
    vector<Customer> activeCustomers;

    const string ROOM_FILE = "rooms.txt";
    const string CUSTOMER_FILE = "customers.txt";

    int invalidInput(int n)
    {
        if (n > 0)
            return n;
        cout << "Invalid input! Enter a positive number: ";
        cin >> n;
        return invalidInput(n);
    }

    // --- FILE HANDLING METHODS ---

    void saveRoomsToFile()
    {
        ofstream outFile(ROOM_FILE);
        if (!outFile)
        {
            cout << "[Error] Could not open rooms file for writing.\n";
            return;
        }
        for (const auto &room : rooms)
        {
            // Format: RoomNumber RoomType PricePerNight IsAvailable(0/1)
            outFile << room.getRoomNumber() << " "
                    << room.getRoomType() << " "
                    << room.getPricePerNight() << " "
                    << room.getAvailability() << "\n";
        }
        outFile.close();
    }

    void loadRoomsFromFile()
    {
        ifstream inFile(ROOM_FILE);
        if (!inFile)
            return; // File might not exist yet on first run

        rooms.clear();
        Room::resetCounters();

        int num;
        string type;
        double price;
        bool available;

        while (inFile >> num >> type >> price >> available)
        {
            rooms.push_back(Room(num, type, price, available));
        }
        inFile.close();
    }

    void saveCustomersToFile()
    {
        ofstream outFile(CUSTOMER_FILE);
        if (!outFile)
        {
            cout << "[Error] Could not open customers file for writing.\n";
            return;
        }
        for (const auto &cust : activeCustomers)
        {
            // Replace spaces in names with underscores for safe file parsing
            string safeName = cust.getName();
            for (char &c : safeName)
                if (c == ' ')
                    c = '_';

            // Format: SafeName PhoneNumber BookedRoomNumber StayDays
            outFile << safeName << " "
                    << cust.getPhoneNumber() << " "
                    << cust.getBookedRoomNumber() << " "
                    << cust.getStayDays() << "\n";
        }
        outFile.close();
    }

    void loadCustomersFromFile()
    {
        ifstream inFile(CUSTOMER_FILE);
        if (!inFile)
            return;

        activeCustomers.clear();

        string name, phone;
        int roomNo, days;

        while (inFile >> name >> phone >> roomNo >> days)
        {
            // Restore underscores back to spaces in customer name
            for (char &c : name)
                if (c == '_')
                    c = ' ';
            activeCustomers.push_back(Customer(name, phone, roomNo, days));
        }
        inFile.close();
    }

public:
    HotelManager()
    {
        // Automatically load persistent data when the program starts
        loadRoomsFromFile();
        loadCustomersFromFile();
    }

    void RoomEntry()
    {
        int roomCount;
        cout << "\n-------- ENTER ROOM DETAILS -------\n";
        cout << "Enter number of rooms you want to insert: ";
        cin >> roomCount;

        for (int i = 0; i < roomCount; i++)
        {
            int roomN;
            string roomtype;
            double price;

            cout << i + 1 << ") ROOM NO :- ";
            cin >> roomN;
            roomN = invalidInput(roomN);

            cout << i + 1 << ") ROOM TYPE (e.g., Single/Double/Suite) :- ";
            cin >> roomtype;

            cout << i + 1 << ") PRICE PER NIGHT ($) :- ";
            cin >> price;
            price = (price > 0) ? price : 100.0;

            rooms.push_back(Room(roomN, roomtype, price, true));
        }

        saveRoomsToFile(); // Save updated inventory to file
        cout << "Rooms added and saved to file successfully!\n";
    }

    void displayRooms() const
    {
        if (rooms.empty())
        {
            cout << "\nNo rooms have been entered yet!\n";
            return;
        }

        cout << "\n---- ROOM INVENTORY DETAILS -----\n";
        for (size_t i = 0; i < rooms.size(); i++)
        {
            cout << "Room NO       - " << rooms[i].getRoomNumber() << "\n";
            cout << "Room Type     - " << rooms[i].getRoomType() << "\n";
            cout << "Price/Night   - $" << fixed << setprecision(2) << rooms[i].getPricePerNight() << "\n";
            cout << "Available     - " << (rooms[i].getAvailability() ? "Yes" : "No") << "\n";
            cout << "-----------------------------\n";
        }
    }

    // --- CUSTOMER SERVICE & IN-HOUSE GUEST PORTAL ---
    void customerServicePortal()
    {
        if (activeCustomers.empty())
        {
            cout << "\n[Notice] There are no active checked-in guests at the moment.\n";
            return;
        }

        int targetRoomNo;
        cout << "\n========================================\n";
        cout << "      CUSTOMER SERVICE & HELP DESK      \n";
        cout << "========================================\n";
        cout << "Enter your currently occupied Room Number: ";
        cin >> targetRoomNo;

        // Verify if the room has an active customer
        Customer *currentGuest = nullptr;
        for (auto &cust : activeCustomers)
        {
            if (cust.getBookedRoomNumber() == targetRoomNo)
            {
                currentGuest = &cust;
                break;
            }
        }

        if (currentGuest == nullptr)
        {
            cout << "\n[ERROR] No active guest found in Room " << targetRoomNo << ".\n";
            return;
        }

        // Service Menu for the Guest
        int serviceChoice;
        do
        {
            cout << "\n--- Welcome, " << currentGuest->getName() << "! (Room " << targetRoomNo << ") ---\n";
            cout << "1. View My Booking & Bill Summary\n";
            cout << "2. Request Room Service / Amenities\n";
            cout << "3. Report a Maintenance Issue\n";
            cout << "4. Back to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> serviceChoice;

            switch (serviceChoice)
            {
            case 1:
            {
                // Find room price for calculation
                for (const auto &room : rooms)
                {
                    if (room.getRoomNumber() == targetRoomNo)
                    {
                        double subtotal = room.getPricePerNight() * currentGuest->getStayDays();
                        double tax = subtotal * 0.12;
                        cout << "\n--- YOUR STAY SUMMARY ---\n";
                        currentGuest->displayUser();
                        cout << "Room Type      : " << room.getRoomType() << "\n";
                        cout << "Stay Duration  : " << currentGuest->getStayDays() << " night(s)\n";
                        cout << "Current Total  : $" << fixed << setprecision(2) << (subtotal + tax) << " (incl. tax)\n";
                        break;
                    }
                }
                break;
            }
            case 2:
            {
                string request;
                cout << "What would you like to request? (e.g., Extra Towels, Dinner): ";
                cin >> ws;
                getline(cin, request);
                cout << "\n[Success] Your request for \"" << request << "\" has been sent to housekeeping!\n";
                break;
            }
            case 3:
            {
                string issue;
                cout << "Describe the issue (e.g., AC not cooling, Wi-Fi down): ";
                cin >> ws;
                getline(cin, issue);
                cout << "\n[Success] Maintenance ticket created for \"" << issue << "\". Staff will assist you shortly.\n";
                break;
            }
            case 4:
                cout << "Returning to main menu...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (serviceChoice != 4);
    }

    void checkIn()
    {
        if (rooms.empty())
        {
            cout << "\nNo rooms available in the system yet. Add rooms first.\n";
            return;
        }

        displayRooms();

        int targetRoomNo;
        cout << "\n--- HOTEL CHECK-IN PORTAL ---\n";
        cout << "Enter room number you want to check-in/book: ";
        cin >> targetRoomNo;

        for (auto &room : rooms)
        {
            if (room.getRoomNumber() == targetRoomNo)
            {
                if (room.getAvailability())
                {
                    Customer newCustomer;
                    newCustomer.setDetails();

                    int days;
                    cout << "Enter number of days of stay: ";
                    cin >> days;
                    days = invalidInput(days);
                    newCustomer.setStayDays(days);
                    newCustomer.setBookedRoomNumber(targetRoomNo);

                    room.setAvailability(false);
                    activeCustomers.push_back(newCustomer);

                    // Save state changes to files
                    saveRoomsToFile();
                    saveCustomersToFile();

                    cout << "\n[SUCCESS] Room " << targetRoomNo << " successfully checked-in for " << newCustomer.getName() << "!\n";
                    return;
                }
                else
                {
                    cout << "\n[ERROR] Room " << targetRoomNo << " is already occupied.\n";
                    return;
                }
            }
        }
        cout << "\n[ERROR] Room " << targetRoomNo << " not found in the system.\n";
    }

    void checkOutAndBilling()
    {
        if (activeCustomers.empty())
        {
            cout << "\nNo active checked-in customers found.\n";
            return;
        }

        int targetRoomNo;
        cout << "\n--- HOTEL CHECK-OUT & BILLING PORTAL ---\n";
        cout << "Enter room number for check-out: ";
        cin >> targetRoomNo;

        for (auto it = activeCustomers.begin(); it != activeCustomers.end(); ++it)
        {
            if (it->getBookedRoomNumber() == targetRoomNo)
            {
                for (auto &room : rooms)
                {
                    if (room.getRoomNumber() == targetRoomNo)
                    {
                        double roomRate = room.getPricePerNight();
                        int days = it->getStayDays();
                        double subtotal = roomRate * days;
                        double tax = subtotal * 0.12;
                        double totalBill = subtotal + tax;

                        cout << "\n========================================\n";
                        cout << "           HOTEL INVOICE BILL           \n";
                        cout << "========================================\n";
                        it->displayUser();
                        cout << "Room Number    : " << targetRoomNo << " (" << room.getRoomType() << ")\n";
                        cout << "Stay Duration  : " << days << " night(s)\n";
                        cout << "Rate per Night : $" << fixed << setprecision(2) << roomRate << "\n";
                        cout << "----------------------------------------\n";
                        cout << "Subtotal       : $" << subtotal << "\n";
                        cout << "Tax (12%)      : $" << tax << "\n";
                        cout << "TOTAL AMOUNT   : $" << totalBill << "\n";
                        cout << "========================================\n";

                        char confirmPay;
                        cout << "Proceed with payment & check-out? (y/n): ";
                        cin >> confirmPay;

                        if (confirmPay == 'y' || confirmPay == 'Y')
                        {
                            room.setAvailability(true);
                            activeCustomers.erase(it);

                            // Update files after check-out
                            saveRoomsToFile();
                            saveCustomersToFile();

                            cout << "\n[SUCCESS] Payment received! Check-out complete. Room " << targetRoomNo << " is now available.\n";
                        }
                        else
                        {
                            cout << "\n[CANCELLED] Check-out aborted. Payment pending.\n";
                        }
                        return;
                    }
                }
            }
        }
        cout << "\n[ERROR] No active booking found for Room " << targetRoomNo << ".\n";
    }

    void displayHotelStats() const
    {
        cout << "\n========== HOTEL STATISTICS ==========\n";
        cout << "Total Rooms in Hotel : " << Room::getTotalRooms() << "\n";
        cout << "Available Rooms      : " << Room::getAvailableRoomsCount() << "\n";
        cout << "Occupied Rooms       : " << (Room::getTotalRooms() - Room::getAvailableRoomsCount()) << "\n";
        cout << "======================================\n";
    }
};

int main()
{
    HotelManager manager;
    int choice2;

    cout << "1. for manager" << endl;
    cout << "2. for user" << endl;
    cout << "ENTER YOUR CHOICE :- ";
    cin >> choice2;
    switch (choice2)
    {
    case 1:
        cout << "for manager" << endl;
        int choice1;
        do
        {
            cout << "\n=== HOTEL MANAGEMENT SYSTEM ===";
            cout << "\n1. Add Rooms";
            cout << "\n2. Process Check-In (Desk Staff)";
            cout << "\n3. Check-Out & Bill Payment";
            cout << "\n4. View Hotel Statistics";
            cout << "\n5. Exit";
            cout << "\nEnter your choice: ";
            cin >> choice1;

            switch (choice1)
            {
            case 1:
                manager.RoomEntry();
                break;

            case 2:
                manager.checkIn();
                break;
            case 3:
                manager.checkOutAndBilling();
                break;
            case 4:
                manager.displayHotelStats();
                break;
            case 5:
                cout << "\nThank you for using the Hotel Management System!\n";
                break;
            default:
                cout << "\nInvalid choice! Please try again.\n";
            }
        } while (choice1 != 5);
        break;
    case 2:
        cout << "for user" << endl;
        do
        {
            cout << "\n=== HOTEL MANAGEMENT SYSTEM ===";
            cout << "\n1. View All Rooms Inventory";
            cout << "\n2. Book Available Rooms";
            cout << "\n3. Exit";
            cout << "\nEnter your choice: ";
            cin >> choice1;

            switch (choice1)
            {
            case 1:
                manager.displayRooms();
                break;

            case 2:
               manager.checkIn();
                break;
            default:
                cout << "\nInvalid choice! Please try again.\n";
            }
        } while (choice1 != 3);
        break;
    default:
        cout << "Invalid input" << endl;
    }

    return 0;
}

