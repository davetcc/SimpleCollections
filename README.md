# SimpleCollections for Arduino and mbed summary

Dave Cherry / TheCodersCorner.com make this library available for you to use. It takes me significant effort to keep all my libraries current and working on a wide range of boards. Please consider making at least a one off donation via the sponsor button if you find it useful. In forks, please keep text to here intact.

This library provides a btree-list implementation that can be used as a straight list, but it is always associative. BtreeList works on a very wide range of boards from Uno right up to most mbed devices. It's benefit for library writers especially is the very wide range of devices it can target with low memory requirements on the smallest of boards.

Why? Because on many embedded boards std lib is simply not available, and on others it is potentially a bit too heavy at runtime. This collection is designed to run on anything from Uno upwards with reasonable performance. 

This class has been expanded upon and broken out from IoAbstraction, as such it has been battle tested in IoAbstraction and TcMenu. You should be aware that use across multiple FreeRTOS tasks or threads on larger boards will require suitable mutex lock. Usage during interrupts is not recommended.

There is a forum where questions can be asked, but the rules of engagement are: **this is my hobby, I make it available because it helps others**. Don't expect immediate answers, make sure you've recreated the problem in a simple sketch that you can send to me. Please consider making at least a one time donation using the sponsor link above before using the forum. 

* [TCC Libraries community discussion forum](https://www.thecoderscorner.com/jforum/)
* I also monitor the Arduino forum [https://forum.arduino.cc/], Arduino related questions can be asked there too.

## Installation for Arduino IDE

This library is available in library manager on both Arduino and PlatformIO, this is the best choice for most people. It is highly recommended that you install the libraries using your library manager.

## Installation for PlatformIO (Arduino or mbed)

Use the platformIO library manager to get the library. It's called 'SimpleCollections'.

## Creating and using a list

We must understand that this list is associative and sorted by a key, it is based on a binary search algorithm, so it is relatively slow to insert into the underlying array as it will need to be inserted into the array at the right point. However, in return for this, lookup by key is very fast - in big-O notation it is approximately Log(N) or in simple terms to look up in a 256 item list by key would take maximum 8 iterations. However, insertion carries a possible copy penalty if the items need reordering.

### Restrictions on what you put in the list

This list works by copying items into the list, so the things you store must follow a couple of simple rules.

The key type can be any type that is 4 bytes or fewer. This is a limitation of the underlying way we implement the storage, to significantly reduce compiled sizes on smaller boards. For example, it could be `int`, `uint32_t`, `unit8_t` etc.

* The type must have a default constructor and a copy constructor.
* The type must have an assignment operator
* It must expose a `getKey` method that returns the key type and marked as const.
* It is best to stick to quite simple classes, as during insert operations they will be copied.
* If you want to use this as a general purpose list and are not interested in ordering, just make getKey return a larger number for each item you add.

## Quick start - create a list, iterate, get by key

Contents of the iteration example to get you started, you can either copy into your ide or open the iteration example. In short, first we create the MyStorage type that will be stored in the list, it has a key of type uint8_t. We then create the list object, populating it in the `setup()` method. In the loop we then read back the values using various techniques.

    #include <Arduino.h>
    #include <SimpleCollections.h>

    class MyStorage {
    private:
        uint8_t key;
        uint32_t value;
    public:
        // we must define
        MyStorage() = default;
        MyStorage(const MyStorage& other) = default;
        MyStorage& operator=(const MyStorage& other) = default;
    
        MyStorage(uint8_t key, uint32_t value) : key(key), value(value) {}
    
        uint8_t getKey() const { return key; }
        uint32_t getValue() const { return value; }
    };

    BtreeList<uint8_t, MyStorage> myList;

    void setup() {
        myList.add(MyStorage(0, 2093));
        myList.add(MyStorage(1, 0xf00dface));
        myList.add(MyStorage(2, 0xdeadbeef));
    }

    void loop() {
        Serial.println("Range iteration");
        for(auto item : myList) {
            Serial.println(item.getValue());
        }

        Serial.println("ForEach iteration");
        myList.forEachItem([] (MyStorage* storage) {
            Serial.println(storage->getValue());
        });
    
        auto item = myList.getByKey(2);
        if(item) {
            Serial.println("Get By Key");
            Serial.println(item->getValue());
        }
        else {
            Serial.println("Get By Key Failed");
        }
    
        Serial.println("Count and capacity");
        Serial.println(myList.count());
        Serial.println(myList.capacity());
        delay(4000);
    }

## List sizing and defaults

On Uno, the initial number of items is lowered to 5 by default, with grow mode set to grow by 5 each time, you can lower this in the constructor if needed. On MEGA2560 it will start with 10 items, and grow by 5 each time. On all 32 bit boards it will start at 10 and double each time. To change the default use the following method

    BtreeList<KeyType, Value> myList(size, howToGrow)

Where the size is the initial capacity of the list, and the grow by mode is one of: `GROW_NEVER, GROW_BY_5, GROW_BY_DOUBLE`

## Other helpful methods

    bsize_t nearestLocation(const K& key) // get the location nearet to key

    const V* items() // get the underlying item array.

    V* itemAtIndex(bsize_t idx) // get the item at a particular index

    bsize_t count() // the number of items in the list

    bsize_t capacity() // the current allocated size of the array

## Platforms known to work

| Platform | Board / Arch   | State            |
|----------|----------------|------------------|
| Arduino  | Nano 33 BLE    | Examples run     |
| Arduino  | Uno, MEGA, AVR | Examples run     |
| Arduino  | SAMD MKR1300   | Examples run     |
| Arduino  | SAMD Seeed     | Examples run     |
| Arduino  | ESP8266        | Awaiting         |
| Arduino  | ESP32          | Awaiting         |
| mbed     | STM32F4        | Ran mbed example |

## Making changes to SimpleCollections

We welcome people rolling up their sleeves and helping out, but please do reach out to us before starting any work, so we can ensure its in sync with our development. We use platformIO for development and have a specific project available to help you get started, along with tests that check many elements still work as expected. See [https://github.com/davetcc/tcLibraryDev]