#ifndef Consts_h__
#define Consts_h__

// 画面解像度
static const float RESOLUTION_X = 640;
static const float RESOLUTION_Y = 960;

// Windows表示時の画面倍率
static const float ZOOM_FACTOR_FOR_WINDOWS = 0.7f;

// 列数
static const int COLUMN_COUNT = 6;
// 行数
static const int ROW_COUNT = 5;

// 属性の数
static const int ATTRIBUTE_NUM = 6;

// 消すのに最低限必要な数
static const int REQUIED_MATCH_NUM = 3;

// 全体攻撃に必要なドロップ数
static const int ATTACK_TO_ALL_NUM = 5;

// ドロップを移動可能な時間（秒）
static const float MAX_TOUCH_DURATION = 50.0f;

// ドロップのサイズ(ピクセル)
static const float DROP_SIZE = 100;
// ドラッグ中のドロップのサイズ比
static const float DRAGGING_DROP_SIZE_RATIO = 1.15f;
// ドロップの画面下端のマージン
static const float DROP_BOTTOM_MARGIN = 20;

// ドロップが消える際（コンボ）の間隔（秒）
static const float REMOVE_DROP_INTERVAL = 0.4f;
// ドロップが落ちるのに要する時間
static const float FALL_DROP_DURATION = 0.2f;
// ドロップが落ちてからチェックするまでの時間
static const float DELAY_AFTER_FALL_DROP = 0.2f;
// コンボで攻撃力がチャージされる間隔（秒）
static const float CHARGE_BY_COMBO_DURATION = 0.5f;
// 攻撃力チャージエフェクトの時間（秒）
static const float ATTACK_CHAGE_EFFECT_DURATION = 0.25f;
// 味方メンバー同士の攻撃間隔（秒）
static const float MEMBERS_ATTACK_INTERVAL = 0.15f;
// 味方の攻撃エフェクト開始から着弾までの時間（秒）
static const float ATTACK_EFFECT_DURATION = 0.3f;
// パーティのライフゲージが減るのにかかる時間
static const float PARTY_APPLY_DAMAGE_DURATION = 0.5f;
// 敵パーティのy座標
static const float ENEMY_PARTY_Y = 700;
// 敵1体攻撃間隔（秒）
static const float ENEMY_ATTACK_INTERVAL = 0.5f;
// 敵の攻撃エフェクトの時間（秒）
static const float ENEMY_ATTACK_EFFECT_DURATION = 0.2f;
// 敵の攻撃を受けてライフゲージが減るのに要する時間（秒）
static const float ENEMY_ATTACK_DAMAGE_DURATION = 0.3f;



// ドロップのタグ
static const int TAG_DROP = 10000;

// コンボ表示のタグ
static const int TAG_COMBO_LABEL = 20000;

static const int TAG_SHAKE_ACTION = 30000;

enum Z_ORDER
{
	Z_BG,
	Z_ENEMY,
	Z_ENEMY_DAMAGE,
	Z_PARTY,
	Z_PARTY_ATTACK_EFFECT,
	Z_LIFE_GAUGE,
	Z_CURE_POWER_LABEL,
	Z_PARTY_DAMAGE,
	Z_DROP,
	Z_SWAPPING_DROP,
	Z_DRAGGING_DROP,
	Z_COMBO,
	Z_CHAGE_ATTACK_EFFECT,
};

#endif // Consts_h__