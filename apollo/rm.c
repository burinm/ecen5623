#include <stdio.h>
#include <math.h>

float least_upper_bound(int num_services);

int main() {
    for (int i=1; i < 100; i++) {
        printf("%d %llf\n", i, least_upper_bound(i));
    }

}

float least_upper_bound(int num_services) {
    return  (num_services * ( pow(2, (float)1 / num_services) - 1) * 100);
}
