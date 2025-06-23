#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
    void setGameScore(int s) { gameScore = s; }
    int getGameScore() const { return gameScore; }

protected:
    ModelListener* modelListener;

private:
    int gameScore = 0;
};

#endif // MODEL_HPP
