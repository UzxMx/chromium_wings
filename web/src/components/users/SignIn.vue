<template>
  <div>
    <el-form class="form-sign-in" :model="user" :rules="rules" ref="formSignIn" label-width="120px">
      <el-form-item label="用户名" prop="username">
        <el-input v-model="user.username"></el-input>
      </el-form-item>

      <el-form-item label="密码" prop="password">
        <el-input type="password" v-model="user.password"></el-input>
      </el-form-item>

      <el-form-item>
        <el-button type="primary" native-type="submit" @click.prevent="handleSignIn">登录</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
import { mapGetters, mapActions } from 'vuex'
import { Loading } from 'element-ui'

export default {
  data () {
    return {
      user: {
        username: '',
        password: ''
      },
      rules: {
        username: [
          { required: true, message: '请输入用户名' }
        ],
        password: [
          { required: true, message: '请输入密码' }
        ]
      }
    }
  },

  computed: mapGetters(['isAuthenticated']),

  methods: {
    ...mapActions(['signIn', 'getTemplates']),

    executeAfterAuthenticated () {
      this.$router.replace({ name: 'Templates.Index' })
    },

    handleSignIn () {
      this.$refs.formSignIn.validate(valid => {
        if (valid) {
          let loading = Loading.service()
          this.signIn(this.user).then(resp => {
            if (this.isAuthenticated) {
              this.executeAfterAuthenticated()
            }
          }).finally(() => loading.close())
        }
      })
    }
  },

  mounted () {
    if (this.isAuthenticated) {
      this.executeAfterAuthenticated()
    }
  }
}
</script>

<style lang="scss" scoped>
  .form-sign-in {
    width: 500px;
    margin: 0 auto;
    padding-top: 150px;
  }
</style>
