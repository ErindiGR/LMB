
# LMB

LMB is a c++ libaray that bakes lightmaps.
The library is flexible and extendable and it alsow supports multithreading.\
LMB supports direct lighting, ambient occlussion,
indirect lighting, denoising and more to come in the 
future.You can also extend the features on your own fairly easily.


# Third Party Libraries

* Imgui:
    * link: https://github.com/ocornut/imgui
    * license (MIT): https://github.com/ocornut/imgui/blob/master/LICENSE.txt
* Glm:
    * link: https://github.com/g-truc/glm
    * license (MIT): https://github.com/g-truc/glm/blob/master/manual.md#section0
* OBJ loader:
    * link: https://github.com/Bly7/OBJ-Loader
    * license (MIT): https://github.com/Bly7/OBJ-Loader/blob/master/LICENSE.txt


# Known Bugs

* None for now (does not mean there are not any).


# Upcoming Features

todo

# Build

Release build:

    $ sh build_release.sh

Debug build:

    $ sh build_debug.sh


# Usage

Example how to calculate ao:

```cpp

int main()
{
    //initiate library
    LMB::Init();

    //create a new lmb session
    auto lmb_session = 
    std::make_shared<LMB::LMBSession>();

    //create a solver
    auto solver = 
    std::make_shared<LMB::GridSolver>(10);

    auto calc_ao =
    std::make_shared<LMB::AOCalculator>(default_ao_config);

    //set the blending function
    calc_ao->SetBlend(std::make_shared<LMB::CalcBlendSet>());

    lmb_session->SetSolver(solver);
    lmb_session->SetCalculator(calc_ao);

    //starts calculating
    lmb_session->StartCalc();

    //waits for the end of calculation
    lmb_session->EndCalc();

    //you can use lmb_session->GetLightmaps() to
    //get the final lightmaps


    //terminate library
    LMB::Term();

}

```
