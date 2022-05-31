#include <iostream>
#include <future>
#include <string>
#include <chrono>
#include <condition_variable>

using namespace std;

void producer_a(promise<string> prm) {
    this_thread::sleep_for(chrono::milliseconds(1000));
    prm.set_value("Hola!!!");
    // Otras tareas...
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Fin de producer_a\n";
}

void consumer_a(future<string> fut) {
    cout << "El valor de x es: " << fut.get() << endl;
}

void producer_b(string& x) {
    this_thread::sleep_for(chrono::milliseconds(1000));
    x = "Hola";
    // Otras tareas...
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Fin de producer_a\n";
}

void consumer_b(string& x) {
    cout << "El valor de x es: "<< x << endl;
}

mutex mtx;
condition_variable cv;
void producer_c(string& x) {
    this_thread::sleep_for(chrono::milliseconds(1000));
    unique_lock ul(mtx);
    x = "Hola";
    ul.unlock();
    cv.notify_one();
    // Otras tareas...
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Fin de producer_a\n";
}

void consumer_c(string& x) {
    unique_lock ul(mtx);
    cv.wait (ul, [&x](){ return !x.empty(); });
    cout << "El valor de x es: "<< x << endl;
}

void ejemplo_1() {
    promise<string> prm_1;
    future<string> fut_1 = prm_1.get_future();
    promise<int> prm_2;
    future<int> fut_2 = prm_2.get_future();

    // En el caso del promise, al momento de generar el mensaje
    // promise utiliza el método set_value(T value) para poderlo enviarlo al future
    prm_1.set_value("Hola!!");
    // El future recepciona el mensaje por medio del metodo get()
    cout << fut_1.get() << endl;

    prm_2.set_value(20);
    cout << fut_2.get() << endl;
}

void ejemplo_2() {
    // 1. Crear el promise
    promise<string> prm;
    // 2. A partir del promise generar el future
    future<string> fut = prm.get_future();
    // Mensaje
    cout << "Inicio de ejemplo 2\n";
    // Crear un hilo que sea el productor y el hilo consumidor
    thread t1(producer_a, move(prm));
    thread t2(consumer_a, move(fut));
    t1.join();
    t2.join();
}

void ejemplo_3() {
    cout << "Inicio de ejemplo 2\n";
    // Crear un hilo que sea el productor y el hilo consumidor
    string x;
    thread t1(producer_b, ref(x));
    thread t2(consumer_b, ref(x));
    t1.join();
    t2.join();
    cout << "El valor de x es: " << x << endl;
}

void ejemplo_4() {
    string x;
    thread t1(producer_c, ref(x));
    thread t2(consumer_c, ref(x));
    t1.join();
    t2.join();
}

int sumar(int x, int y) {
    return x + y;
}

void ejemplo_5() {
    auto result = async(sumar, 10, 20); // result no es un entero, sino un future
    cout << result.get() << endl;
}

int main() {
//    ejemplo_2();
//    ejemplo_3();
//    ejemplo_4();
    ejemplo_5();
    return 0;
}
