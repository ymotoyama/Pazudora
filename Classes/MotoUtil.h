#ifndef MOTO_UTIL_H
#define MOTO_UTIL_H

#include "cocos2d.h"
#include <random>

USING_NS_CC;

// 画面サイズ（Size）
#define WINSIZE (Director::getInstance()->getWinSize())
// 画面の高さ
#define WIN_HEIGHT (Director::getInstance()->getWinSize().height)
// 画面の幅
#define WIN_WIDTH (Director::getInstance()->getWinSize().width)
// 画面の中央（x）
#define WIN_CENTER_X (Director::getInstance()->getWinSize().width / 2)
// 画面の中央（y）
#define WIN_CENTER_Y (Director::getInstance()->getWinSize().height / 2)
// 画面の中央の座標
#define WIN_CENTER (Vec2(WIN_CENTER_X, WIN_CENTER_Y))
// Directorのインスタンスを取得する
#define DIRECTOR (Director::getInstance())

// 引数に渡したNodeの位置（スクリーン座標系）を取得する
#define GET_SCREEN_POS(node) (node->convertToWorldSpaceAR(Point::ZERO))


// 関数およびラムダ式を一定時間後に実行（Actionを返却する）
#define CREATE_EXEC_DELAY(delayTime, callbackLambda) (Sequence::create(DelayTime::create(delayTime), CallFunc::create(callbackLambda), nullptr))
// 関数およびラムダ式を一定時間後に実行（第3引数のNodeのアクションとして実行する）
#define EXEC_DELAY(delayTime, callbackLambda, node) (node->runAction(Sequence::create(DelayTime::create(delayTime), CallFunc::create(callbackLambda), nullptr)))
// 直列アクション（Sequence）を生成する。末尾にNULLは不要
#define SEQUENCE(...) (Sequence::create(__VA_ARGS__, nullptr))
// 並列アクション（Spawn）を生成する。末尾にNULLは不要
#define SPAWN(...) (Spawn::create(__VA_ARGS__, nullptr))
// DelayTime アクション
#define DELAY(duration) (DelayTime::create(duration))
// フェードアウト後に消滅するアクション
#define FADEOUT_AND_REMOVE(duration) (Sequence::create(FadeOut::create(duration), RemoveSelf::create(), nullptr))


#define DEFAULT_ANIMATION_DURATION (0.4f)


// Nodeを振動させるAction
class Shake : public ActionInterval
{
public:
	// duration : アニメーションの長さ
	// range : 振動の範囲。y=0にすれば横揺れ、x=0にすれば縦揺れとなる
	// speedRate : 振動の速さ。1が標準で、大きいほど激しい
	static Shake* create(float duration, const Vec2& range, float speedRate = 1.0f);

	// Overrides
	virtual Shake* clone() const override;
	virtual Shake* reverse(void) const  override;
	virtual void startWithTarget(Node *target) override;
	virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
	Shake() {}
	virtual ~Shake() {}

	/** initializes the action */
	bool initWithDuration(float duration, const Vec2& range, float speedRate);

protected:
	Vec2 _startPosition;
	Vec2 _range;
	float _speedRate;

	int _rotation;
	int _count;
	int _nextRotationCount;
	float _theta[4];

private:

};


// ラムダ式を渡すことにより、何でもできるアクション
class CustomAction : public ActionInterval
{
public:
	// duration : アニメーションの長さ
	// from : 開始時の値
	// to : 目的の値
	// callbackLambda : 毎フレーム呼ばれるラムダ式。戻り値無し、float型の引数を1つとる。引数にはその瞬間にとるべき値が入ってくる。
	static CustomAction* create(float duration, float from, float to, std::function<void(float)> callbackLambda);

	// Overrides
	virtual CustomAction* clone() const override;
	virtual CustomAction* reverse(void) const  override;
	virtual void startWithTarget(Node *target) override;
	virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
	CustomAction() {}
	virtual ~CustomAction() {}

	/** initializes the action */
	bool initWithDuration(float duration, float from, float to, std::function<void(float)> callbackLambda);

protected:
private:
	std::function<void(float)> _callback;
	float _from;
	float _to;
};

#define RAND (Random::getInstance())
#define RAND_INT(min, max) (RAND->getInt(min, max))
#define RAND_0_1() (RAND->get0_1())
#define RAND_MINUS1_1 (RAND->getMinus1_1())
#define RAND_FLOAT(min, max) (RAND->getFloat(min, max))
#define RAND_ND(avarage, sigma) (RAND->getNormalDistribution(avarage, sigma))

// 乱数生成オブジェクト（シングルトン）
class Random
{
public:
	Random();
	Random(long seed);
	virtual ~Random();

	// 乱数生成オブジェクトのインスタンスを返却する
	static Random* getInstance();

	// 乱数のシードを設定する
	static void init(long seed);

	// 現在使われている乱数のシード値を返却する
	long getSeed();

	// (signed)int型の値を返却する
	int getInt();

	// 指定した範囲のint型の値を返却する（maxを含む）
	int getInt(int min, int max);

	// 0～1のfloat型の値を返却する
	float get0_1();

	// -1～+1のfloat型の値を返却する
	float getMinus1_1();

	// 指定した範囲のfloat型の値を返却する
	float getFloat(float min, float max);

	// 正規分布に基づき乱数を返却する
	// average : 平均値
	// sigma : 標準偏差
	float getNormalDistribution(float average, float sigma);

	// ランダムでtrueまたはfalseを返却する
	bool getBool();

	// 入力した確率でtrueを返却する
	// percent : 確率。0から100の値を入力してください。
	bool probability(float percent);
	
private :
	static Random* s_instance;
	std::mt19937* _mt;
	long _seed;
};

// 引数valueを指定した範囲に収める
int clamp(int value, int min, int max);

// フェードアウト/インでシーンを切り替える
void replaceSceneFade(Scene* scene, float duration = 1.0f);


bool isIOS();
bool isAndroid();
bool isWin32();

// addChildすると同時に、親の中央に位置するようにchildをsetPositionする
void addChildToCenter(Node* parent, Node* child);

// 指定したActionを強制終了する。
// targetは直ちに、Actionが最後まで実行された状態となる。
// Actionは削除される。
void forceFinishActionByTag(Node* target, int tag);


#endif